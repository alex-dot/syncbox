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
void *subscriber_thread(void *arg);

Boxoffice* Boxoffice::initialize(zmq::context_t* z_ctx)
{
  Boxoffice* bo = getInstance();

  // TODO: serialize settings -> subscribers
  bo->subscribers.push_back(std::make_pair(
    "ipc://syncbox.ipc",SB_SUBTYPE_TCP_BIDIR));
  bo->subscribers.push_back(std::make_pair(
    "ipc://syncbox.ipc",SB_SUBTYPE_TCP_BIDIR));

  // setting up
  bo->setContext(z_ctx);
  bo->connectToMain();

  // setting up the publisher
  bo->setupPublisher();
  bo->connectToPub();

  // setting up the subscribers, the router and aggregate the subs
  // note that the Watchers are all subscribers too!
  bo->setupSubscribers();
  bo->runRouter();

  // closing
  bo->closeConnections();

  return bo;
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
  std::cout << "bo: opening publisher thread" << std::endl;
  pub_thread = new boost::thread(publisher_thread, z_ctx);

  return 0;
}
int Boxoffice::connectToPub()
{
  // standard variables
  zmq::message_t z_msg;
  int msg_type, msg_signal;
  std::stringstream sstream;

  // since the publisher is a singleton, we can simply use a ZMQ_PAIR
  z_pub_pair = new zmq::socket_t(*z_ctx, ZMQ_PAIR);
  z_pub_pair->connect("inproc://sb_pub_bo_pair");

  // wait for heartbeat from publisher
  std::cout << "bo: waiting for publisher to send heartbeat" << std::endl;
  z_pub_pair->recv(&z_msg);
  sstream.clear();
  sstream << static_cast<char*>(z_msg.data());
  sstream >> msg_type >> msg_signal;
  if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_ALIVE ) return 1;

  // sending publisher channel list
  std::cout << "bo: waiting to send publisher channel list" << std::endl;
  z_pub_pair->recv(&z_msg);
  sstream.clear();
  sstream << static_cast<char*>(z_msg.data());
  sstream >> msg_type >> msg_signal;
  if ( msg_type != SB_SIGTYPE_PUB || msg_signal != SB_SIGPUB_GET_CHANNELS ) return 1;
  else {
    std::cout << "bo: sending publisher channel list" << std::endl;
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

  return 0;
}

int Boxoffice::setupSubscribers()
{
  // opening subscriber threads
  std::cout << "bo: opening subscriber threads" << std::endl;
  for (unsigned int i = 0; i < subscribers.size(); ++i)
  {
    boost::thread* sub_thread = new boost::thread(subscriber_thread, z_ctx);
    sub_threads.push_back(sub_thread);
  }

  return 0;
}

int Boxoffice::runRouter()
{
  // standard variables
  zmq::message_t z_msg;
  int msg_type, msg_signal;
  std::stringstream sstream;

  // connect to subscribers
  z_router = new zmq::socket_t(*z_ctx, ZMQ_PULL);
  z_router->bind("inproc://sb_boxoffice_subs");

  std::cout << "bo: starting to listen to subs..." << std::endl;

  // wait for heartbeat
  for (unsigned int i = 0; i < subscribers.size(); ++i)
  {
    z_router->recv(&z_msg);
    sstream.str(std::string());
    sstream << static_cast<char*>(z_msg.data());
    std::cout << "bo: pull: " << sstream.str() << std::endl;
    z_msg.rebuild();
  }

  // wait for exit signal
  for (unsigned int i = 0; i < subscribers.size(); ++i)
  {
    z_router->recv(&z_msg);
    sstream.str(std::string());
    sstream << static_cast<char*>(z_msg.data());
    std::cout << "bo: pull: " << sstream.str() << std::endl;
    z_msg.rebuild();
  }

  // wait for exit signal from publisher
  std::cout << "bo: waiting for publisher to send exit signal" << std::endl;
  z_pub_pair->recv(&z_msg);
  sstream.clear();
  sstream << static_cast<char*>(z_msg.data());
  sstream >> msg_type >> msg_signal;

  return 0;
}

int Boxoffice::closeConnections()
{
  // closing PAIR to pub
  z_pub_pair->close();

  // sending exit signal to the main thread...
  std::cout << "bo: sending exit signal..." << std::endl;
  zmq::message_t z_msg_close(3);
  snprintf((char*) z_msg_close.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
  z_bo_main->send(z_msg_close);
  // ...and exiting
  z_bo_main->close();

  // closing publisher and subscriber threads
  pub_thread->join();
  for (std::vector<boost::thread*>::iterator i = sub_threads.begin(); i != sub_threads.end(); ++i)
    (*i)->join();

  std::cout << "bo: exit signal sent, exiting..." << std::endl;

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

void *subscriber_thread(void *arg)
{
  // actually, since Subscriber is not a singleton, we could use the ctor,
  // but to have consistency we still implement initialize()
  Subscriber* sub;
  sub = Subscriber::initialize(static_cast<zmq::context_t*>(arg));

  sub->run();
  sub->sendExitSignal();

  return (NULL);
}