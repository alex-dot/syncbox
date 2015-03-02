/*
 * Boxoffice
 */

#include <zmq.hpp>
#include <boost/thread.hpp>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility>

#include "constants.hpp"
#include "boxoffice.hpp"
#include "publisher.hpp"
#include "subscriber.hpp"

void *publisher_thread(void *arg);
void *subscriber_thread(zmq::context_t*, std::string, int);

Boxoffice* Boxoffice::initialize(zmq::context_t* z_ctx)
{
  int return_value = 0;

  Boxoffice* bo = getInstance();

  // TODO: serialize settings -> subscribers
  bo->subscribers.push_back(std::make_pair(
    "ipc://syncbox.ipc",SB_SUBTYPE_TCP_BIDIR));

  // setting up
  return_value = bo->setContext(z_ctx);
  if (return_value == 0) return_value = bo->connectToBroadcast();
  if (return_value == 0) return_value = bo->connectToMain();

  // setting up the publisher
  if (return_value == 0) return_value = bo->setupPublisher();
  if (return_value == 0) return_value = bo->connectToPub();

  // setting up the subscribers, the router and aggregate the subs
  // note that the Watchers are all subscribers too!
  if (return_value == 0) return_value = bo->setupSubscribers();
  if (return_value == 0) return_value = bo->runRouter();

  // closing
  bo->closeConnections(return_value);

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

int Boxoffice::setupPublisher()
{
  // open publisher thread
  if (SB_MSG_DEBUG) printf("bo: opening publisher thread\n");
  pub_thread = new boost::thread(publisher_thread, z_ctx);

  return 0;
}
int Boxoffice::connectToPub()
{
  // standard variables
  zmq::message_t z_msg;
  std::istringstream* isstream; // to reuse the same name, we have to use pointers
  int msg_type, msg_signal;

  // since the publisher is a singleton, we can simply use a ZMQ_PAIR
  z_pub_pair = new zmq::socket_t(*z_ctx, ZMQ_PAIR);
  z_pub_pair->connect("inproc://sb_pub_bo_pair");

  // wait for heartbeat from publisher
  if (SB_MSG_DEBUG) printf("bo: waiting for publisher to send heartbeat\n");
  isstream = new std::istringstream();
  isstream->str(std::string(s_recv(*z_pub_pair, *z_broadcast)));
  *isstream >> msg_type >> msg_signal;
  if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_ALIVE ) return 1;
  delete isstream;

  // sending publisher channel list
  if (SB_MSG_DEBUG) printf("bo: waiting to send publisher channel list\n");
  isstream = new std::istringstream();
  isstream->str(std::string(s_recv(*z_pub_pair, *z_broadcast)));
  *isstream >> msg_type >> msg_signal;
  if ( msg_type != SB_SIGTYPE_PUB || msg_signal != SB_SIGPUB_GET_CHANNELS ) return 1;
  else {
    if (SB_MSG_DEBUG) printf("bo: sending publisher channel list\n");
    std::vector<std::string> channel_list;
    channel_list.push_back("ipc://syncbox.ipc");
    for (std::vector<std::string>::iterator i = channel_list.begin(); i != channel_list.end(); ++i)
    {
      memcpy(z_msg.data(), i->data(), i->size()+1);
      if ( (*i) != channel_list.back() )
        z_pub_pair->send(z_msg, ZMQ_SNDMORE);
      else
        z_pub_pair->send(z_msg, 0); 
    }
  }
  delete isstream;

  return 0;
}

int Boxoffice::setupSubscribers()
{
  // opening subscriber threads
  if (SB_MSG_DEBUG) printf("bo: opening subscriber threads\n");
  for (std::vector< std::pair<std::string,int> >::iterator i = subscribers.begin(); 
        i != subscribers.end(); ++i)
  {
    boost::thread* sub_thread = new boost::thread(subscriber_thread, z_ctx, i->first, i->second);
    sub_threads.push_back(sub_thread);
  }

  return 0;
}

int Boxoffice::runRouter()
{
  // standard variables
  zmq::message_t z_msg;
  int msg_type, msg_signal;
  std::istringstream* isstream;

  // connect to subscribers
  z_router = new zmq::socket_t(*z_ctx, ZMQ_PULL);
  z_router->bind("inproc://sb_boxoffice_subs");

  if (SB_MSG_DEBUG) printf("bo: starting to listen to subs...\n");

  // wait for heartbeat
  for (unsigned int i = 0; i < subscribers.size(); ++i)
  {
    isstream = new std::istringstream();
    isstream->str(std::string(s_recv(*z_router, *z_broadcast)));
    *isstream >> msg_type >> msg_signal;
    if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_ALIVE ) return 1;
    delete isstream;
  }

  if (SB_MSG_DEBUG) printf("bo: all subscribers connected\n");

  // wait for exit signal
  for (unsigned int i = 0; i < subscribers.size(); ++i)
  {
    isstream = new std::istringstream();
    isstream->str(std::string(s_recv(*z_router, *z_broadcast)));
    *isstream >> msg_type >> msg_signal;
    if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_EXIT ) return 1;
    delete isstream;
  }

  if (SB_MSG_DEBUG) printf("bo: all subscribers exited\n");

  // wait for exit signal from publisher
  if (SB_MSG_DEBUG) printf("bo: waiting for publisher to send exit signal\n");
  isstream = new std::istringstream();
  isstream->str(std::string(s_recv(*z_pub_pair, *z_broadcast)));
  *isstream >> msg_type >> msg_signal;
  if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_EXIT ) return 1;
  delete isstream;
std::cout << "bo: received signal from pub" << std::endl;

  return 0;
}

int Boxoffice::closeConnections(int return_value)
{
  // if return_val != 1, we want to interrupt the publisher and catch the exit signal
  if ( return_value != 0 ) 
  {
    pub_thread->interrupt();

    // query again for exit signal
    if (SB_MSG_DEBUG) printf("bo: waiting for publisher to send exit signal\n");
    std::istringstream* isstream = new std::istringstream();
    int msg_type, msg_signal;
    isstream->str(std::string(s_recv(*z_pub_pair, *z_broadcast)));
    *isstream >> msg_type >> msg_signal;
    if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_EXIT ) return 1;
    delete isstream;
  }
  // closing broadcast socket
  // we check if the sockets are nullptrs, but z_broadcast is never
  z_broadcast->close();

  // closing PAIR to pub
  if ( z_pub_pair != nullptr )
    z_pub_pair->close();

  // closing publisher and subscriber threads
  if ( pub_thread != nullptr )
    pub_thread->join();

  for (std::vector<boost::thread*>::iterator i = sub_threads.begin(); i != sub_threads.end(); ++i)
    (*i)->join();

  if ( z_router != nullptr )
    z_router->close();

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

  return 0;
}


void *publisher_thread(void* arg)
{
  Publisher* pub;
  pub = Publisher::initialize(static_cast<zmq::context_t*>(arg));

  pub->run();
  pub->sendExitSignal();

  return (NULL);
}

void *subscriber_thread(zmq::context_t* z_ctx, std::string endpoint, int sb_subtype)
{
  // actually, since Subscriber is not a singleton, we could use the ctor,
  // but to have consistency we still implement initialize()
  Subscriber* sub;
  sub = Subscriber::initialize(z_ctx, endpoint, sb_subtype);

  sub->run();
  sub->sendExitSignal();

  return (NULL);
}