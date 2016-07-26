/*
 * Dispatcher
 */

#include "constants.hpp"
#include "dispatcher.hpp"

#include <unistd.h>
#include <endian.h>

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread.hpp>
#include <chrono>
#include <thread>

Dispatcher::Dispatcher(zmqpp::context* z_ctx_, fsm::status_t status) :
  Transmitter(z_ctx_),
  z_dispatcher(nullptr),
  z_boxoffice_disp_push(nullptr),
  current_status_(fsm::status_100),
  offset_(-1) {
    tac = (char*)"dis";
    this->connectToBoxofficeHB();
    this->connectToPublisher();
}

Dispatcher::~Dispatcher() {
  z_boxoffice_disp_push->close();
  z_dispatcher->close();

  delete z_boxoffice_disp_push;
  delete z_dispatcher;
}

int Dispatcher::connectToPublisher()
{
  // connect to process broadcast
  z_dispatcher = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pair);
  z_dispatcher->bind("inproc://pub_disp_pair");

  return 0;
}

int Dispatcher::connectToBoxofficeHB()
{
  // open connection to receive HB data from boxoffice
  z_boxoffice_disp_push = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_boxoffice_disp_push->connect("inproc://sb_boxoffice_disp_push_out");
  z_boxoffice_disp_push->subscribe("");

  return 0;
}

int Dispatcher::run()
{
  // internal check if heartbeater was correctly initialized
  if ( z_ctx == nullptr )
    return 1;

  if (SB_MSG_DEBUG) printf("dis: starting disp socket and sending...\n");

  std::stringstream* sstream;
  int msg_type, msg_signal;

  while(true)
  {
    // waiting for boxoffice input in non-blocking mode
    sstream = new std::stringstream();
    int z_return = s_recv_noblock(*z_boxoffice_push, *z_boxoffice_disp_push, *z_broadcast, *sstream, 1);

    if ( z_return != 0 ) {
      *sstream >> msg_type >> msg_signal;
      if ( msg_type == SB_SIGTYPE_LIFE && msg_signal == SB_SIGLIFE_INTERRUPT ) break;
//      if ( msg_type == SB_SIGTYPE_FSM ) {
//        current_status_ = (fsm::status_t)msg_signal;
//        std::getline(*sstream, current_message_);
//      }
    }
/*
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    if (SB_MSG_DEBUG) printf("dis: sending disp status code %d\n", (int)current_status_);

    // send a message
    uint64_t timestamp = htobe64(
      std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::system_clock::now().time_since_epoch()
      ).count()
    );
    char* timestamp_c = new char(8);
    std::memcpy(timestamp_c, &timestamp, 8);

    std::stringstream* message = new std::stringstream();
    *message << SB_SIGTYPE_PUB  << " "
             << current_status_ << " ";
    message->write(timestamp_c, 8);
    *message << current_message_;
    zmqpp::message z_msg;
    z_msg << message->str();
    z_dispatcher->send(z_msg, true);

    delete message;
    delete sstream;
*/
  }

  delete sstream;

  return 0;
}