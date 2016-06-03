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
  delete z_boxoffice_pull;
  delete z_boxoffice_push;
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
  // open connection to send data to boxoffice
  z_boxoffice_pull = new zmq::socket_t(*z_ctx, ZMQ_PUSH);
  z_boxoffice_pull->connect("inproc://sb_boxoffice_pull_in");
  // open connection to receive data from boxoffice
  z_boxoffice_push = new zmq::socket_t(*z_ctx, ZMQ_SUB);
  z_boxoffice_push->connect("inproc://sb_boxoffice_push_out");
  const char *sub_filter = "";
  z_boxoffice_push->setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);

  // send a heartbeat to boxoffice, so it knows the publisher is ready
  if (SB_MSG_DEBUG) printf("pub: sending heartbeat...\n");
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_ALIVE);
  z_boxoffice_pull->send(z_msg);

  return 0;
}

int Publisher::sendExitSignal()
{
  // send exit signal to boxoffice
  if (SB_MSG_DEBUG) printf("pub: sending exit signal...\n");
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
  z_boxoffice_pull->send(z_msg);

  if (SB_MSG_DEBUG) printf("pub: signal sent, exiting...\n");
  z_boxoffice_pull->close();

  z_publisher->close();
  z_broadcast->close();
  z_boxoffice_pull->close();
  z_boxoffice_push->close();

  return 0;
}

int Publisher::run()
{
  // internal check if publisher was correctly initialized
  if ( z_ctx == nullptr || endpoint.compare("") == 0 )
    return 1;

  if (SB_MSG_DEBUG) printf("pub: starting pub socket and sending...\n");
  z_publisher = new zmq::socket_t(*z_ctx, ZMQ_PUB);
  z_publisher->bind(endpoint.c_str());

  std::stringstream* sstream;
  int msg_type, msg_signal;

  while(true)
  {
    // waiting for boxoffice input in non-blocking mode
    sstream = new std::stringstream();
//    int z_return = s_recv_noblock(*z_boxoffice_push, *z_broadcast, *sstream);
    s_recv(*z_boxoffice_push, *z_broadcast, *sstream);

//    if ( z_return > 0 ) {
      *sstream >> msg_type >> msg_signal;
      if ( msg_type == SB_SIGTYPE_LIFE || msg_signal == SB_SIGLIFE_INTERRUPT ) break;
//    }

    // send a message
    std::string message;
    *sstream >> message;
    if (SB_MSG_DEBUG) printf("pub: %s\n", message.c_str());
    zmq::message_t z_msg(message.length()+1);
    snprintf((char*) z_msg.data(), message.length()+1, "%s", message.c_str());
    z_publisher->send(z_msg);

    delete sstream;
  }

  delete sstream;

  return 0;
}