/*
 * Heartbeater
 */

#include "constants.hpp"
#include "heartbeater.hpp"

#include <unistd.h>

#include <zmqpp/zmqpp.hpp>
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

Heartbeater* Heartbeater::initialize(zmqpp::context* z_ctx_, 
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
  z_broadcast = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_broadcast->connect("inproc://sb_broadcast");
  z_broadcast->subscribe("");

  return 0;
}

int Heartbeater::connectToBoxoffice()
{
  // open connection to send data to boxoffice
  z_boxoffice_pull = new zmqpp::socket(*z_ctx, zmqpp::socket_type::push);
  z_boxoffice_pull->connect("inproc://sb_boxoffice_pull_in");
  // open connection to receive data from boxoffice
  z_boxoffice_push = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_boxoffice_push->connect("inproc://sb_boxoffice_hb_push_out");
  z_boxoffice_push->subscribe("");

  // send a heartbeat to boxoffice, so it knows the heartbeater is ready
  if (SB_MSG_DEBUG) printf("hb: sending heartbeat...\n");
  std::stringstream message;
  message << SB_SIGTYPE_LIFE << " " << SB_SIGLIFE_ALIVE;
  zmqpp::message z_msg;
  z_msg << message.str();
  z_boxoffice_pull->send(z_msg);

  return 0;
}

int Heartbeater::sendExitSignal()
{
  // send exit signal to boxoffice
  if (SB_MSG_DEBUG) printf("hb: sending exit signal...\n");
  std::stringstream message;
  message << SB_SIGTYPE_LIFE << " " << SB_SIGLIFE_EXIT;
  zmqpp::message z_msg;
  z_msg << message.str();
  z_boxoffice_pull->send(z_msg);

  if (SB_MSG_DEBUG) printf("hb: signal sent, exiting...\n");

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
  z_heartbeater = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pair);
  // this needs to be opened up for more than one publisher/heartbeater
  z_heartbeater->bind("inproc://pub_hb_pair");

  std::stringstream* sstream;
  int msg_type, msg_signal;

  while(true)
  {
    // waiting for boxoffice input in non-blocking mode
    sstream = new std::stringstream();
    int z_return = s_recv_noblock(*z_boxoffice_push, *z_broadcast, *sstream, 2000);

    if ( z_return != 0 ) {
      *sstream >> msg_type >> msg_signal;
      if ( msg_type == SB_SIGTYPE_LIFE && msg_signal == SB_SIGLIFE_INTERRUPT ) break;
      if ( msg_type == SB_SIGTYPE_FSM ) {
        current_status_ = (fsm::status_t)msg_signal;
      }
    }

    if (SB_MSG_DEBUG) printf("hb: sending hb status code %d\n", (int)current_status_);

    // send a message
    std::string infomessage = "info";
    std::stringstream message;
    message << (int)SB_SIGTYPE_PUB  << " "
            << (int)current_status_ << " "
            << infomessage.c_str();
    zmqpp::message z_msg;
    z_msg << message.str();
    z_heartbeater->send(z_msg);

    delete sstream;
  }

  delete sstream;

  return 0;
}