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

Heartbeater::Heartbeater(zmqpp::context* z_ctx_, fsm::status_t status) :
  Transmitter(z_ctx_),
  z_heartbeater(nullptr), 
  current_status_(status),
  current_message_("") {
    tac = (char*)"hb";
    this->connectToPublisher();
}

Heartbeater::~Heartbeater() {
  z_heartbeater->close();

  delete z_heartbeater;
}

int Heartbeater::connectToPublisher()
{
  // connect to process broadcast
  z_heartbeater = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pair);
  z_heartbeater->connect("inproc://pub_hb_pair");

  return 0;
}

int Heartbeater::run()
{
  // internal check if heartbeater was correctly initialized
  if ( z_ctx == nullptr )
    return 1;

  if (SB_MSG_DEBUG) printf("hb: starting hb socket and sending...\n");

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
        *sstream >> current_message_;
      }
    }

    if (SB_MSG_DEBUG) printf("hb: sending hb status code %d\n", (int)current_status_);

    // send a message
    std::stringstream message;
    message << (int)SB_SIGTYPE_PUB  << " "
            << (int)current_status_ << " "
            << current_message_.c_str();
    zmqpp::message z_msg;
    z_msg << message.str();
    z_heartbeater->send(z_msg);

    delete sstream;
  }

  delete sstream;

  return 0;
}