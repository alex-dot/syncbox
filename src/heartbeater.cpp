/*
 * Heartbeater
 */

#include "constants.hpp"
#include "heartbeater.hpp"

#include <unistd.h>
#include <endian.h>

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread.hpp>
#include <chrono>
#include <thread>

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
  z_heartbeater->bind("inproc://pub_hb_pair");

  return 0;
}

int Heartbeater::connectToBoxofficeHB()
{
  // open connection to receive HB data from boxoffice
  z_boxoffice_hb_push = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_boxoffice_hb_push->connect("inproc://f_boxoffice_hb_push_out");
  z_boxoffice_hb_push->subscribe("");

  return 0;
}

int Heartbeater::run()
{
  // internal check if heartbeater was correctly initialized
  if ( z_ctx == nullptr )
    return 1;

  if (F_MSG_DEBUG) printf("hb: starting hb socket and sending...\n");

  std::stringstream* sstream;
  int msg_type, msg_signal;

  while(true)
  {
    // waiting for boxoffice input in non-blocking mode
    sstream = new std::stringstream();
    int z_return = s_recv_noblock(*z_boxoffice_push, *z_boxoffice_hb_push, *z_broadcast, *sstream, 1);

    if ( z_return != 0 ) {
      *sstream >> msg_type >> msg_signal;
      if ( msg_type == F_SIGTYPE_LIFE && msg_signal == F_SIGLIFE_INTERRUPT ) break;
      if ( msg_type == F_SIGTYPE_FSM ) {
        current_status_ = (fsm::status_t)msg_signal;
        std::getline(*sstream, current_message_);
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    if (F_MSG_DEBUG) printf("hb: sending hb status code %d\n", (int)current_status_);

    // send a message
    uint64_t timestamp = htobe64(
      std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::system_clock::now().time_since_epoch()
      ).count()
    );
    char* timestamp_c = new char(8);
    std::memcpy(timestamp_c, &timestamp, 8);

    std::stringstream* message = new std::stringstream();
    *message << F_SIGTYPE_PUB  << " "
             << current_status_ << " ";
    message->write(timestamp_c, 8);
    *message << current_message_;
    zmqpp::message z_msg;
    z_msg << message->str();
    z_heartbeater->send(z_msg, true);

    delete message;
    delete sstream;
  }

  delete sstream;

  return 0;
}