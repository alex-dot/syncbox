/*
 * Publisher
 */

#include "constants.hpp"
#include "publisher.hpp"

#include <unistd.h>

#include <zmq.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread.hpp>


Publisher::~Publisher()
{
  delete z_boxoffice;
  delete z_publisher;
  delete z_broadcast;
}

Publisher* Publisher::initialize(zmq::context_t* z_ctx_, 
                                 std::string endpoint_)
{
  Publisher* pub = new Publisher(z_ctx_, endpoint_);

  pub->connectToBroadcast();
  pub->connectToBoxoffice();

  return pub;
}

int Publisher::connectToBroadcast()
{
  // connect to process broadcast
  z_broadcast = new zmq::socket_t(*z_ctx, ZMQ_SUB);
  z_broadcast->connect("inproc://sb_broadcast");
  const char *sub_filter = "";
  z_broadcast->setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);

  return 0;
}

int Publisher::connectToBoxoffice()
{
  z_boxoffice = new zmq::socket_t(*z_ctx, ZMQ_PUSH);
  z_boxoffice->connect("inproc://sb_boxoffice_pull_in");

  // send a heartbeat to boxoffice, so it knows the publisher is ready
  if (SB_MSG_DEBUG) printf("pub: sending heartbeat...\n");
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_ALIVE);
  z_boxoffice->send(z_msg);

  return 0;
}

int Publisher::sendExitSignal()
{
  // send exit signal to boxoffice
  if (SB_MSG_DEBUG) printf("pub: sending exit signal...\n");
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
  z_boxoffice->send(z_msg);

  if (SB_MSG_DEBUG) printf("pub: signal sent, exiting...\n");
  z_boxoffice->close();

  z_publisher->close();
  z_broadcast->close();

  return 0;
}

int Publisher::run()
{
  // internal check if publisher was correctly initialized
  if ( z_ctx == nullptr || endpoint.compare("") == 0 )
    return 1;

  if (SB_MSG_DEBUG) printf("pub: starting pub socket and sending...\n");
  zmq::socket_t* z_publisher = new zmq::socket_t(*z_ctx, ZMQ_PUB);
  z_publisher->bind(endpoint.c_str());

  std::stringstream* sstream;
  int msg_type, msg_signal;
  //zmq::socket_t* channel = channel_list.back();

  /*for (int i = 0; i < 2; ++i)
  {
    if (boost::this_thread::interruption_requested()) 
      break;

    std::string message = "this is message: " + std::to_string(i);
    if (SB_MSG_DEBUG) printf("pub: %s\n", message.c_str());
    zmq::message_t z_msg(message.length()+1);
    snprintf((char*) z_msg.data(), message.length()+1, "%s", message.c_str());
    channel->send(z_msg, ZMQ_SNDMORE);
    channel->send(z_msg, ZMQ_SNDMORE);
    channel->send(z_msg);
    sleep(1);
  }*/
  while(true)
  {
    // waiting for boxoffice input
    sstream = new std::stringstream();
    s_recv(*z_boxoffice, *z_broadcast, *sstream);
    *sstream >> msg_type >> msg_signal;
    delete sstream;
    if ( msg_type == SB_SIGTYPE_LIFE || msg_signal == SB_SIGLIFE_INTERRUPT ) break;
  }

  return 0;
}