/*
 * Heartbeater
 */

#include "constants.hpp"
#include "heartbeater.hpp"

#include <unistd.h>

#include <zmq.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread.hpp>


Heartbeater::~Heartbeater()
{
  delete z_boxoffice_pull;
  delete z_boxoffice_push;
  delete z_heartbeater;
  delete z_broadcast;
}

Heartbeater* Heartbeater::initialize(zmq::context_t* z_ctx_, 
                                     fsm::status_t status)
{
  Heartbeater* hb = new Heartbeater(z_ctx_, status);

  hb->connectToBroadcast();
  hb->connectToBoxoffice();

  return hb;
}

int Heartbeater::connectToBroadcast()
{
  // connect to process broadcast
  z_broadcast = new zmq::socket_t(*z_ctx, ZMQ_SUB);
  z_broadcast->connect("inproc://sb_broadcast");
  const char *sub_filter = "";
  z_broadcast->setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);

  return 0;
}

int Heartbeater::connectToBoxoffice()
{
  // open connection to send data to boxoffice
  z_boxoffice_pull = new zmq::socket_t(*z_ctx, ZMQ_PUSH);
  z_boxoffice_pull->connect("inproc://sb_boxoffice_pull_in");
  // open connection to receive data from boxoffice
  z_boxoffice_push = new zmq::socket_t(*z_ctx, ZMQ_SUB);
  z_boxoffice_push->connect("inproc://sb_boxoffice_hb_push_out");
  const char *sub_filter = "";
  z_boxoffice_push->setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);

  // send a heartbeat to boxoffice, so it knows the heartbeater is ready
  if (SB_MSG_DEBUG) printf("hb: sending heartbeat...\n");
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_ALIVE);
  z_boxoffice_pull->send(z_msg);

  return 0;
}

int Heartbeater::sendExitSignal()
{
  // send exit signal to boxoffice
  if (SB_MSG_DEBUG) printf("hb: sending exit signal...\n");
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
  z_boxoffice_pull->send(z_msg);

  if (SB_MSG_DEBUG) printf("hb: signal sent, exiting...\n");
  z_boxoffice_pull->close();

  z_heartbeater->close();
  z_broadcast->close();
  z_boxoffice_pull->close();
  z_boxoffice_push->close();

  return 0;
}

int Heartbeater::run()
{
  // internal check if heartbeater was correctly initialized
  if ( z_ctx == nullptr )
    return 1;

  if (SB_MSG_DEBUG) printf("hb: starting hb socket and sending...\n");
  z_heartbeater = new zmq::socket_t(*z_ctx, ZMQ_PAIR);
  // this needs to be opened up for more than one publisher/heartbeater
  z_heartbeater->bind("inproc://pub_hb_pair");

  std::stringstream* sstream;
  int msg_type, msg_signal;

  while(true)
  {
    // waiting for boxoffice input in non-blocking mode
    sstream = new std::stringstream();
    int z_return = s_recv_noblock(*z_boxoffice_push, *z_broadcast, *sstream, 500);

    if ( z_return > 0 ) {
      *sstream >> msg_type >> msg_signal;
      if ( msg_type == SB_SIGTYPE_LIFE && msg_signal == SB_SIGLIFE_INTERRUPT ) break;
      if ( msg_type == SB_SIGTYPE_FSM ) {
        current_status_ = (fsm::status_t)msg_signal;
      }
    }

    if (SB_MSG_DEBUG) printf("hb: sending hb status code %d\n", (int)current_status_);

    // send a message
    std::string message = "info";
    zmq::message_t z_msg(message.length()+7);
    snprintf(
      (char*) z_msg.data(), 
      message.length()+7, 
      "%d %d %s", 
      SB_SIGTYPE_PUB, 
      (int)current_status_, 
      message.c_str()
    );
    z_heartbeater->send(z_msg);

    delete sstream;
  }

  delete sstream;

  return 0;
}