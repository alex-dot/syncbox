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
  z_boxoffice_hb_push(nullptr),
  current_status_(status),
  current_message_("") {
    tac = (char*)"hb";
    this->connectToBoxofficeHB();
    this->connectToPublisher();
}

Heartbeater::~Heartbeater() {
  z_boxoffice_hb_push->close();
  z_heartbeater->close();

  delete z_boxoffice_hb_push;
  delete z_heartbeater;
}

int Heartbeater::connectToPublisher()
{
  // connect to process broadcast
  z_heartbeater = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pair);
  z_heartbeater->connect("inproc://pub_hb_pair");

  return 0;
}

int Heartbeater::connectToBoxofficeHB()
{
  // open connection to receive HB data from boxoffice
  z_boxoffice_hb_push = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_boxoffice_hb_push->connect("inproc://sb_boxoffice_hb_push_out");
  z_boxoffice_hb_push->subscribe("");

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
    int z_return = s_recv_noblock(*z_boxoffice_push, *z_boxoffice_hb_push, *z_broadcast, *sstream, 2000);

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
    int64_t timestamp = std::chrono::duration_cast< std::chrono::milliseconds >(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
    std::stringstream message;
    message << (int)SB_SIGTYPE_PUB  << " "
            << (int)current_status_ << " "
            << timestamp            << " "
            << current_message_.c_str();
    zmqpp::message z_msg;
    z_msg << message.str();
    z_heartbeater->send(z_msg);

    delete sstream;
  }

  delete sstream;

  return 0;
}