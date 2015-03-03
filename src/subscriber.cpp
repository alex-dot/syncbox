/*
 * Subscriber
 */

#include "constants.hpp"
#include "subscriber.hpp"

#include <unistd.h>

#include <zmq.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread.hpp>

Subscriber* Subscriber::initialize(zmq::context_t* z_ctx_, 
                                   std::string endpoint_, 
                                   int sb_subtype_)
{
  Subscriber* sub = new Subscriber(z_ctx_, endpoint_, sb_subtype_);

  sub->connectToBroadcast();
  sub->connectToBoxoffice();

  return sub;
}

int Subscriber::connectToBroadcast()
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

int Subscriber::connectToBoxoffice()
{
  z_boxoffice = new zmq::socket_t(*z_ctx, ZMQ_PUSH);
  z_boxoffice->connect("inproc://sb_boxoffice_subs");

  // send a heartbeat to boxoffice, so it knows the subscriber is ready
  if (SB_MSG_DEBUG) printf("sub: sending heartbeat...\n");
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_ALIVE);
  z_boxoffice->send(z_msg);

  return 0;
}

int Subscriber::sendExitSignal()
{
  // send exit signal to boxoffice
  if (SB_MSG_DEBUG) printf("sub: sending exit signal...\n");
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
  z_boxoffice->send(z_msg);

  if (SB_MSG_DEBUG) printf("sub: signal sent, exiting...\n");
  z_boxoffice->close();

  z_subscriber->close();
  z_broadcast->close();

  return 0;
}

int Subscriber::run()
{
  // internal check if subscriber was correctly initialized
  if ( z_ctx == nullptr || endpoint.compare("") == 0
      || ( sb_subtype != SB_SUBTYPE_TCP_BIDIR && sb_subtype != SB_SUBTYPE_TCP_UNIDIR ) )
    return 1;

  if (SB_MSG_DEBUG) printf("sub: receiving from publisher...\n");
  z_subscriber = new zmq::socket_t(*z_ctx, ZMQ_SUB);
  z_subscriber->connect(endpoint.c_str());
  const char *sub_filter = "";
  z_subscriber->setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);

  for (int i = 0; i < 1; ++i)
  {
    std::stringstream* sstream = new std::stringstream();
    int msg_type, msg_signal;
    s_recv(*z_subscriber, *z_broadcast, *sstream);
    if (*sstream >> msg_type >> msg_signal) {
      if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_ALIVE ) { return 1; }
        else { return -1; } }
    std::cout << sstream->str() << std::endl;
    delete sstream;
  }

  return 0;
}