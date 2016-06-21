/*
 * Boxoffice
 */

#include <zmq.hpp>
#include <boost/thread.hpp>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include "cryptopp/aes.h"

#include "constants.hpp"
#include "boxoffice.hpp"
#include "publisher.hpp"
#include "heartbeater.hpp"
#include "subscriber.hpp"
#include "config.hpp"

void *publisher_thread(zmq::context_t*, std::string);
void *heartbeater_thread(zmq::context_t*, fsm::status_t status);
void *subscriber_thread(zmq::context_t*, std::string, int);
void *box_thread(zmq::context_t* z_ctx, Box* box);

Boxoffice::~Boxoffice()
{
  // deleting boxes
  for (std::unordered_map<int,Box*>::iterator i = boxes.begin(); i != boxes.end(); ++i)
    delete i->second;
  boxes.clear();

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

Boxoffice* Boxoffice::initialize(zmq::context_t* z_ctx)
{
  int return_value = 0;

  Boxoffice* bo = getInstance();

  Config* conf = Config::getInstance();
  bo->subscribers = conf->getSubscriberEndpoints();
  bo->publishers = conf->getPublisherEndpoints();

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

int Boxoffice::setContext(zmq::context_t* z_ctx_)
{
  zmq::context_t* z_ctx_temp = z_ctx;
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
  z_broadcast = new zmq::socket_t(*z_ctx, ZMQ_SUB);
  z_broadcast->connect("inproc://sb_broadcast");
  const char *sub_filter = "";
  z_broadcast->setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);

  return 0;
}

int Boxoffice::connectToMain()
{
  // open PAIR sender to main thread
  z_bo_main = new zmq::socket_t(*z_ctx, ZMQ_PAIR);
  z_bo_main->connect("inproc://sb_bo_main_pair");

  return 0;
}

int Boxoffice::setupConnectionToChildren()
{
  // connection for information from subscribers, publishers and boxes
  z_router = new zmq::socket_t(*z_ctx, ZMQ_PULL);
  z_router->bind("inproc://sb_boxoffice_pull_in");
  // connection to send information to publishers and boxes
  z_bo_pub = new zmq::socket_t(*z_ctx, ZMQ_PUB);
  z_bo_pub->bind("inproc://sb_boxoffice_push_out");
  // connection to send information to heartbeaters
  z_bo_hb = new zmq::socket_t(*z_ctx, ZMQ_PUB);
  z_bo_hb->bind("inproc://sb_boxoffice_hb_push_out");
  if (SB_MSG_DEBUG) printf("bo: starting to listen to children...\n");

  return 0;
}

int Boxoffice::setupBoxes()
{
  Config* conf = Config::getInstance();
  std::vector<std::string> box_dirs = conf->getBoxDirectories();

  int box_num = 0;
  box_threads.reserve(box_dirs.size());
  for (std::vector<std::string>::iterator i = box_dirs.begin(); i != box_dirs.end(); ++i)
  {
    // initializing the boxes here, so we can use file IO while it's thread 
    // still listens to inotify events
    Box* box = new Box(*i, box_num);
    boxes.insert(std::make_pair(box_num,box));
    ++box_num;
    // opening box thread
    boost::thread* bt = new boost::thread(box_thread, z_ctx, box);
    box_threads.push_back(bt);
  }

  return 0;
}

int Boxoffice::setupPublishers()
{
  // standard variables
  zmq::message_t z_msg;

  // opening publisher threads
  if (SB_MSG_DEBUG) printf("bo: opening %d publisher threads\n", (int)publishers.size());
  for (std::vector< std::string >::iterator i = publishers.begin(); 
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
  zmq::message_t z_msg;

  // opening heartbeater threads
  if (SB_MSG_DEBUG) printf("bo: opening %d heartbeater threads\n", (int)publishers.size());
  for (std::vector< std::string >::iterator i = publishers.begin(); 
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
  for (std::vector< std::pair<std::string,int> >::iterator i = subscribers.begin(); 
        i != subscribers.end(); ++i)
  {
    boost::thread* sub_thread = new boost::thread(subscriber_thread, z_ctx, i->first, i->second);
    sub_threads.push_back(sub_thread);
  }
  if (SB_MSG_DEBUG) printf("bo: opened %d subscriber threads\n", (int)sub_threads.size());

  return 0;
}

int Boxoffice::checkChildren() {
  // standard variables
  zmq::message_t z_msg;
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

int Boxoffice::runRouter()
{ 
  // standard variables
  zmq::message_t z_msg;
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

    if ( msg_type == SB_SIGTYPE_INOTIFY || msg_type == SB_SIGTYPE_SUB ) {

      // fsm variables
      fsm::status_t status = (fsm::status_t)msg_signal;
      fsm::event_t event = fsm::get_event_by_status_code(status);

      if (SB_MSG_DEBUG) printf("bo: checking event with state %d, event %d and status %d\n", 
        state_, event, status);
      if ( fsm::check_event(state_, event, status) ) {
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

    }

    delete sstream;
  }

  return 0;
}

int Boxoffice::closeConnections()
{
  // standard variables
  int msg_type, msg_signal;
  std::stringstream* sstream;
  int return_value = 0;

  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
  z_bo_main->send(z_msg);

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

/*  // if return_val != 0, the pub got interrupted, so it will sent the exit signal again
  if ( return_value != 0 ) 
  {
    // query again for exit signal
    if (SB_MSG_DEBUG) printf("bo: waiting for publisher to send exit signal\n");
    std::stringstream* sstream = new std::stringstream();
    int msg_type, msg_signal;
    s_recv(*z_pub_pair, *z_broadcast, *sstream);
    *sstream >> msg_type >> msg_signal;
    if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_EXIT ) return 1;
    delete sstream;
  }*/

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
  if ( z_bo_main != nullptr )
  {
    if (SB_MSG_DEBUG) printf("bo: sending exit signal...\n");
    zmq::message_t z_msg_close(3);
    snprintf((char*) z_msg_close.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
    z_bo_main->send(z_msg_close);
    // ...and exiting
    z_bo_main->close();
  }

  if (SB_MSG_DEBUG) printf("bo: exit signal sent, exiting...\n");

  return return_value;
}


int Boxoffice::performAction(fsm::event_t event, fsm::action_t action, fsm::status_t received_status) {

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

  /*
  // helper variables
  int box_num, wd, cookie;
  uint32_t mask;
  std::string filename, line;
  std::stringstream* sstream_line;
  std::string path;

  // reading inotify message
  sstream->seekg(0, std::stringstream::beg);
  std::getline(*sstream, line);      // signal frame
  sstream_line = new std::stringstream(line);
  *sstream_line >> msg_type >> msg_signal >> box_num;
  delete sstream_line;
  std::getline(*sstream, line);      // info frame
  sstream_line = new std::stringstream(line);
  *sstream_line >> wd >> mask >> cookie;
  delete sstream_line;
  std::getline(*sstream, filename);  // filename frame

  // getting the path
  Box* box = boxes[box_num];
  std::string file_path = box->getBaseDir();
  file_path += box->getPathOfDirectory(wd);
  if ( file_path.back() != '/' ) file_path += "/";
  file_path += filename;

  if (SB_MSG_DEBUG) printf("bo: %s\n", file_path.c_str());

  zmq::message_t z_msg( file_path.length()+6 );
  snprintf(
    (char*) z_msg.data(), 
    file_path.length()+7, 
    "%d %d %s", 
    SB_SIGTYPE_FSM, 
    new_status, 
    file_path.c_str());
  z_bo_pub->send(z_msg);
  */

  return 0;
}

int Boxoffice::updateHeartbeat(fsm::status_t new_status) {
  if (SB_MSG_DEBUG) printf("bo: changing status code to %d\n", new_status);
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 6, "%d %d", SB_SIGTYPE_FSM, new_status);
  z_bo_hb->send(z_msg);

  return 0;
}



void *publisher_thread(zmq::context_t* z_ctx, std::string endpoint)
{
  Publisher* pub;
  pub = Publisher::initialize(z_ctx, endpoint);

  pub->run();
  pub->sendExitSignal();

  delete pub;

  return (NULL);
}

void *heartbeater_thread(zmq::context_t* z_ctx, fsm::status_t status)
{
  Heartbeater* hb;
  hb = Heartbeater::initialize(z_ctx, status);

  hb->run();
  hb->sendExitSignal();

  delete hb;

  return (NULL);
}

void *subscriber_thread(zmq::context_t* z_ctx, std::string endpoint, int sb_subtype)
{
  Subscriber* sub;
  sub = Subscriber::initialize(z_ctx, endpoint, sb_subtype);

  sub->run();
  sub->sendExitSignal();

  delete sub;

  return (NULL);
}

void *box_thread(zmq::context_t* z_ctx, Box* box)
{
  // although boxes have their own thread, it is only used for catching inotify
  // signals
  box->setContext(z_ctx);
  box->connectToBroadcast();
  box->connectToBoxoffice();
  box->watch();
  box->sendExitSignal();

  return (NULL);
}