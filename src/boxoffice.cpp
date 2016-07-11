/*
 * Boxoffice
 */

#include <zmqpp/zmqpp.hpp>
#include <boost/thread.hpp>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <sys/inotify.h>
#include "cryptopp/aes.h"

#include "constants.hpp"
#include "boxoffice.hpp"
#include "publisher.hpp"
#include "heartbeater.hpp"
#include "subscriber.hpp"

void *publisher_thread(zmqpp::context*, host_t host);
void *heartbeater_thread(zmqpp::context*, fsm::status_t status);
void *subscriber_thread(zmqpp::context*, node_t node);
void *box_thread(Box* box);

Boxoffice::~Boxoffice()
{
  // deleting threads
  for (std::vector<boost::thread*>::iterator i = pub_threads.begin(); i != pub_threads.end(); ++i)
    delete *i;
  for (std::vector<boost::thread*>::iterator i =  hb_threads.begin(); i !=  hb_threads.end(); ++i)
    delete *i;
  for (std::vector<boost::thread*>::iterator i = sub_threads.begin(); i != sub_threads.end(); ++i)
    delete *i;
  for (std::vector<boost::thread*>::iterator i = box_threads.begin(); i != box_threads.end(); ++i)
    delete *i;

  // deleting sockets
  delete z_bo_main;
  delete z_router;
  delete z_bo_pub;
  delete z_bo_hb;
  delete z_broadcast;
}

Boxoffice* Boxoffice::initialize(zmqpp::context* z_ctx)
{
  int return_value = 0;

  Boxoffice* bo = getInstance();

  Config* conf = Config::getInstance();
  bo->subscribers = conf->getNodes();
  bo->publishers = conf->getHosts();

  // setting up
  return_value = bo->setContext(z_ctx);
  if (return_value == 0) return_value = bo->connectToBroadcast();
  if (return_value == 0) return_value = bo->connectToMain();

  // setting up the subscribers and publishers, the router and aggregate the subs and pubs
  // note that the Watchers are all subscribers too!
  if (return_value == 0) return_value = bo->setupConnectionToChildren();
  if (return_value == 0) return_value = bo->setupBoxes();
  if (return_value == 0) return_value = bo->setupPublishers();
  if (return_value == 0) return_value = bo->setupHeartbeaters();
  if (return_value == 0) return_value = bo->setupSubscribers();
  if (return_value == 0) return_value = bo->checkChildren();
  if (return_value == 0) return_value = bo->runRouter();

  // closing
  bo->closeConnections();

  return bo;
}

int Boxoffice::setContext(zmqpp::context* z_ctx_)
{
  zmqpp::context* z_ctx_temp = z_ctx;
  z_ctx = z_ctx_;
      
  if ( z_ctx_temp == nullptr ) return 0;
  else return 1;

  return -1;
}

int Boxoffice::connectToBroadcast()
{
  // connect to process broadcast
  // since we want to rarely use that (mostly interrupt) we assume that by 
  // the time we need it, we have long been connected to it
  z_broadcast = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_broadcast->connect("inproc://sb_broadcast");
  z_broadcast->subscribe("");

  return 0;
}

int Boxoffice::connectToMain()
{
  // open PAIR sender to main thread
  z_bo_main = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pair);
  z_bo_main->connect("inproc://sb_bo_main_pair");

  return 0;
}

int Boxoffice::setupConnectionToChildren()
{
  // connection for information from subscribers, publishers and boxes
  z_router = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pull);
  z_router->bind("inproc://sb_boxoffice_pull_in");
  // connection to send information to publishers and boxes
  z_bo_pub = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pub);
  z_bo_pub->bind("inproc://sb_boxoffice_push_out");
  // connection to send information to heartbeaters
  z_bo_hb = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pub);
  z_bo_hb->bind("inproc://sb_boxoffice_hb_push_out");
  if (SB_MSG_DEBUG) printf("bo: starting to listen to children...\n");

  return 0;
}

int Boxoffice::setupBoxes()
{
  Config* conf = Config::getInstance();
  std::vector< box_t > box_dirs = conf->getBoxes();

  box_threads.reserve(box_dirs.size());
  if (SB_MSG_DEBUG) printf("bo: opening %d box threads\n", (int)box_dirs.size());
  for (std::vector<box_t>::iterator i = box_dirs.begin(); i != box_dirs.end(); ++i)
  {
    // initializing the boxes here, so we can use file IO while it's thread 
    // still listens to inotify events
    Box* box = new Box(z_ctx, i->base_path, i->uid);
    boxes.insert(std::make_pair(i->uid,box));

    // opening box thread
    boost::thread* bt = new boost::thread(box_thread, box);
    box_threads.push_back(bt);
  }
  if (SB_MSG_DEBUG) printf("bo: opened %d box threads\n", (int)box_dirs.size());

  return 0;
}

int Boxoffice::setupPublishers()
{
  // standard variables
  zmqpp::message z_msg;

  // opening publisher threads
  if (SB_MSG_DEBUG) printf("bo: opening %d publisher threads\n", (int)publishers.size());
  for (std::vector< host_t >::iterator i = publishers.begin(); 
        i != publishers.end(); ++i)
  {
    boost::thread* pub_thread = new boost::thread(publisher_thread, z_ctx, *i);
    pub_threads.push_back(pub_thread);
  }
  if (SB_MSG_DEBUG) printf("bo: opened %d publisher threads\n", (int)pub_threads.size());

  return 0;
}

int Boxoffice::setupHeartbeaters()
{
  // standard variables
  zmqpp::message z_msg;

  // opening heartbeater threads
  if (SB_MSG_DEBUG) printf("bo: opening %d heartbeater threads\n", (int)publishers.size());
  for (std::vector< host_t >::iterator i = publishers.begin(); 
        i != publishers.end(); ++i)
  {
    boost::thread* hb_thread = new boost::thread(heartbeater_thread, z_ctx, fsm::status_100);
    hb_threads.push_back(hb_thread);
  }
  if (SB_MSG_DEBUG) printf("bo: opened %d heartbeater threads\n", (int)hb_threads.size());

  return 0;
}

int Boxoffice::setupSubscribers()
{
  // opening subscriber threads
  if (SB_MSG_DEBUG) printf("bo: opening %d subscriber threads\n", (int)subscribers.size());
  for (std::unordered_map< std::string, node_t >::iterator i = subscribers.begin(); 
        i != subscribers.end(); ++i)
  {
    boost::thread* sub_thread = new boost::thread(subscriber_thread, 
                                                  z_ctx, 
                                                  i->second);
    sub_threads.push_back(sub_thread);
    ++total_node_number_;
  }
  if (SB_MSG_DEBUG) printf("bo: opened %d subscriber threads\n", (int)sub_threads.size());

  return 0;
}

int Boxoffice::checkChildren() {
  // standard variables
  zmqpp::message z_msg;
  int msg_type, msg_signal;
  std::stringstream* sstream;

  // wait for heartbeat
  int heartbeats = sub_threads.size() + pub_threads.size() + hb_threads.size() + box_threads.size();
  for (int i = 0; i < heartbeats; ++i)
  {
    sstream = new std::stringstream();
    s_recv(*z_router, *z_broadcast, *sstream);
    *sstream >> msg_type >> msg_signal;
    if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_ALIVE ) return 1;
    delete sstream;
  }
  if (SB_MSG_DEBUG) printf("bo: all subscribers, publishers, heartbeaters and boxes connected\n");
  if (SB_MSG_DEBUG) printf("bo: counted %d threads\n", heartbeats);

  return 0;
}

int Boxoffice::closeConnections()
{
  // standard variables
  int msg_type, msg_signal;
  std::stringstream* sstream;
  int return_value = 0;

  // wait for exit/interrupt signal
  int heartbeats = sub_threads.size() + pub_threads.size() + hb_threads.size() + box_threads.size();
  for (int i = 0; i < heartbeats; ++i)
  {
    sstream = new std::stringstream();
    s_recv(*z_router, *z_broadcast, *sstream);
    *sstream >> msg_type >> msg_signal;
    if ( msg_type != SB_SIGTYPE_LIFE || (   msg_signal != SB_SIGLIFE_EXIT
                                         && msg_signal != SB_SIGLIFE_INTERRUPT ) ) return_value = 1;
    delete sstream;
  }
  if (SB_MSG_DEBUG) printf("bo: all subscribers, publishers, heartbeaters and boxes exited\n");

  // closing broadcast socket
  // we check if the sockets are nullptrs, but z_broadcast is never
  z_broadcast->close();

  // closing publisher, subscriber and box threads
  for (std::vector<boost::thread*>::iterator i = pub_threads.begin(); i != pub_threads.end(); ++i)
    (*i)->join();

  for (std::vector<boost::thread*>::iterator i =  hb_threads.begin(); i !=  hb_threads.end(); ++i)
    (*i)->join();

  for (std::vector<boost::thread*>::iterator i = sub_threads.begin(); i != sub_threads.end(); ++i)
    (*i)->join();

  for (std::vector<boost::thread*>::iterator i = box_threads.begin(); i != box_threads.end(); ++i)
    (*i)->join();

  if ( z_router != nullptr )
    z_router->close();

  z_bo_pub->close();
  z_bo_hb->close();

  // sending exit signal to the main thread...
  if (SB_MSG_DEBUG) printf("bo: sending exit signal...\n");
  std::stringstream message;
  message << SB_SIGTYPE_LIFE << " " << SB_SIGLIFE_EXIT;
  zmqpp::message z_msg;
  z_msg << message.str();
  z_bo_main->send(z_msg);
  // ...and exiting
  z_bo_main->close();

  if (SB_MSG_DEBUG) printf("bo: exit signal sent, exiting...\n");

  return return_value;
}

int Boxoffice::runRouter()
{ 
  // standard variables
  zmqpp::message z_msg;
  int msg_type, msg_signal;
  std::stringstream* sstream;

  if (SB_MSG_DEBUG) printf("bo: waiting for input from subscribers and watchers\n");
  while(true)
  {
    // waiting for subscriber or inotify input
    sstream = new std::stringstream();
    s_recv(*z_router, *z_bo_main, *sstream);
    *sstream >> msg_type >> msg_signal;
    if ( msg_type == SB_SIGTYPE_LIFE    && msg_signal == SB_SIGLIFE_INTERRUPT ) 
    {
      delete sstream;
      break;
    }

    if ( msg_type == SB_SIGTYPE_INOTIFY ) {
      std::string box_hash, path;
      *sstream >> box_hash >> path;
      std::cout << path << std::endl;
      processEvent((fsm::status_t)msg_signal, path);
    } else if ( msg_type == SB_SIGTYPE_SUB ) {
      std::string message = sstream->str();
      processEvent((fsm::status_t)msg_signal, message);
    }

    delete sstream;
  }

  return 0;
}

int Boxoffice::processEvent(fsm::status_t status, std::string const message) {
  fsm::event_t event = fsm::get_event_by_status_code(status);

  if (SB_MSG_DEBUG) printf("bo: checking event with state %d, event %d and status %d\n", 
    state_, event, status);
  if ( fsm::check_event(state_, event, status) ) {

    // RECEIVED_HEARTBEAT_EVENT
    if ( event == fsm::received_heartbeat_event ) {
      switch ( status ) {
        // STATUS_100
        // if the received status was simply 100, just update the corresponding node
        case fsm::status_100: {
          std::stringstream sstream;
          sstream << message;
          int msg_type, msg_signal;
          std::string node_uid;
          int64_t node_timestamp, local_timestamp;
          int16_t offset;
          sstream >> msg_type >> msg_signal >> node_uid >> node_timestamp;

          subscribers[node_uid].last_timestamp = node_timestamp;
          local_timestamp = std::chrono::duration_cast< std::chrono::milliseconds >(
            std::chrono::system_clock::now().time_since_epoch()
          ).count();
          offset = local_timestamp - subscribers[node_uid].last_timestamp;
          subscribers[node_uid].offset = offset;

          break;
        }
        // STATUS_121
        // waiting for all nodes to reply, so increment node_reply_counter_
        // until all nodes replied, then manually change the status
        case fsm::status_121: {
          ++node_reply_counter_;

          if ( node_reply_counter_ == total_node_number_ ) {
            node_reply_counter_ = -1;
            status = fsm::status_122;
            event = fsm::get_event_by_status_code(status);
            if ( !fsm::check_event(state_, event, status) ) {
              if (SB_MSG_DEBUG) printf("bo: changed to unhandled event, ignoring...\n");
              return 1;
            }
          }

          break;
        }

        default:
          break;
        }
    }

    // NEW_LOCAL_FILE_EVENT
    // if the event was new_local_file_event, add the path of the file to the queue;
    // if the state was announcing_new_file_state, we need to check if the reported 
    // file is the same as the last one, otherwise we would push the same file
    // twice; TODO if a file was created and immediately deleted, that file should 
    // be removed and - if necessary - the state should be reverted
    if ( event == fsm::new_local_file_event
      && state_ == fsm::announcing_new_file_state ) {
      std::vector<std::string>::iterator iter;
      iter = std::find( file_list_.begin(), file_list_.end(), message );
      if ( iter != file_list_.end() ) {
        file_list_.push_back(message);
      }
      node_reply_counter_ = 0;
    } else if ( event == fsm::new_local_file_event ) {
      file_list_.push_back(message);
      node_reply_counter_ = 0;
    }


    // FSM CONTINUE
    fsm::state_t new_state = fsm::get_new_state(state_, event, status);
    if ( state_ != new_state ) {
      fsm::action_t action = fsm::get_action(state_, event, status);
      performAction(event, action, status);
      if (SB_MSG_DEBUG) printf("bo: updating self to state %d\n", 
        fsm::get_new_state(state_, event, status));
      state_ = new_state;
    }

  } else {
    if (SB_MSG_DEBUG) printf("bo: unhandled event, ignoring...\n");
  }

  return 0;
}

int Boxoffice::performAction(fsm::event_t const event, 
                             fsm::action_t const action, 
                             fsm::status_t const received_status) const {

  switch (action) {
    case fsm::send_heartbeat_action: {
      fsm::status_t new_status = fsm::get_heartbeat_status(state_, event, received_status);
      updateHeartbeat(new_status);
      break;
    }
    default:
      if (SB_MSG_DEBUG) printf("bo: could not perform action: %d\n", action);
      break;
  }

  return 0;
}

int Boxoffice::updateHeartbeat(fsm::status_t const new_status) const {
  if (SB_MSG_DEBUG) printf("bo: changing status code to %d\n", new_status);
  std::stringstream message;
  message << SB_SIGTYPE_FSM << " " << new_status << " ";
  prepareHeartbeatMessage(message);
  zmqpp::message z_msg;
  z_msg << message.str();
  z_bo_hb->send(z_msg);

  return 0;
}

void Boxoffice::prepareHeartbeatMessage(std::stringstream& message) const {
  message << "";
}



void *publisher_thread(zmqpp::context* z_ctx, host_t host)
{
  Publisher* pub = new Publisher(z_ctx, host);

  pub->run();
  pub->sendExitSignal();

  delete pub;

  return (NULL);
}

void *heartbeater_thread(zmqpp::context* z_ctx, fsm::status_t status)
{
  Heartbeater* hb = new Heartbeater(z_ctx, status);

  hb->run();
  hb->sendExitSignal();

  delete hb;

  return (NULL);
}

void *subscriber_thread(zmqpp::context* z_ctx, node_t node)
{
  Subscriber* sub = new Subscriber(z_ctx, node);

  sub->run();
  sub->sendExitSignal();

  delete sub;

  return (NULL);
}

void *box_thread(Box* box)
{
  box->run();
  box->sendExitSignal();

  delete box;

  return (NULL);
}