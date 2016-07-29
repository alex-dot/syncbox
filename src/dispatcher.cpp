/*
 * Dispatcher
 */

#include "constants.hpp"
#include "dispatcher.hpp"
#include "file.hpp"

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
  current_status_(status),
  timing_offset_(-1) {
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
    s_recv(*z_boxoffice_push, *z_boxoffice_disp_push, *z_broadcast, *sstream);

    *sstream >> msg_type >> msg_signal;
    if ( msg_type == SB_SIGTYPE_LIFE && msg_signal == SB_SIGLIFE_INTERRUPT ) break;
    if ( msg_type == SB_SIGTYPE_FSM ) {
      current_status_ = (fsm::status_t)msg_signal;
    }

    if (SB_MSG_DEBUG) printf("dis: sending disp status code %d\n", (int)current_status_);

    if ( current_status_ == fsm::status_122
      || current_status_ == fsm::status_177 ) {
      uint32_t timing_offset;
      char* timing_offset_c = new char[4];
      sstream->seekg(1, std::ios_base::cur);
      sstream->read(timing_offset_c, 4);
      std::memcpy(&timing_offset, timing_offset_c, 4);
      timing_offset_ = be32toh(timing_offset);

// \TODO needs individual offset
// \TODO needs DEBUG conditional
//      std::this_thread::sleep_for(std::chrono::milliseconds(timing_offset_));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      std::stringstream* message = new std::stringstream();
      *message << SB_SIGTYPE_FSM  << " "
               << fsm::status_132;
      zmqpp::message* z_msg = new zmqpp::message();
      *z_msg << message->str();
      z_boxoffice_pull->send(*z_msg);
      delete message;
      message = new std::stringstream();
      delete z_msg;
      z_msg = new zmqpp::message();

      current_status_ = fsm::status_200;

      char box_hash_s[SB_GENERIC_HASH_LEN];
      sstream->read(box_hash_s, SB_GENERIC_HASH_LEN);
      unsigned char box_hash[SB_GENERIC_HASH_LEN];
      std::memcpy(box_hash, box_hash_s, SB_GENERIC_HASH_LEN);
      Hash* hash = new Hash(box_hash);
      std::string box_dir;
      *sstream >> box_dir;

      File* file = new File(box_dir, hash);
      sstream->seekg(1, std::ios_base::cur);
      *sstream >> *file;

      char* contents = new char[SB_MAXIMUM_FILE_PACKAGE_SIZE];
      bool* more = new bool(true);
      uint64_t data_size = 0;
      uint64_t offset = 0;

      message = new std::stringstream();
      *message << SB_SIGTYPE_PUB  << " "
               << current_status_;
      z_msg = new zmqpp::message();

      while (*more) {
*more = false;
/*
        data_size += file->readFileData(contents,
                                         SB_MAXIMUM_FILE_PACKAGE_SIZE,
                                         offset,
                                         more);
        offset += SB_MAXIMUM_FILE_PACKAGE_SIZE;

        message->write(contents, data_size);
        *z_msg << message->str();
        z_dispatcher->send(*z_msg);
*/

        delete contents;
        contents = new char[SB_MAXIMUM_FILE_PACKAGE_SIZE];
        delete message;
        message = new std::stringstream();
        delete z_msg;
        z_msg = new zmqpp::message();
      }
      delete contents;
      delete message;
      delete z_msg;
    } else if (current_status_ == fsm::status_130) {
      uint32_t timing_offset;
      char* timing_offset_c = new char[4];
      sstream->seekg(1, std::ios_base::cur);
      sstream->read(timing_offset_c, 4);
      std::memcpy(&timing_offset, timing_offset_c, 4);
      timing_offset_ = be32toh(timing_offset);

// \TODO needs individual offset
// \TODO needs DEBUG conditional
//      std::this_thread::sleep_for(std::chrono::milliseconds(timing_offset_));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      std::stringstream* message = new std::stringstream();
      *message << SB_SIGTYPE_FSM  << " "
               << fsm::status_132;
      zmqpp::message* z_msg = new zmqpp::message();
      *z_msg << message->str();
      z_boxoffice_pull->send(*z_msg);
      delete message;
      message = new std::stringstream();
      delete z_msg;
      z_msg = new zmqpp::message();

      current_status_ = fsm::status_132;

      message = new std::stringstream();
      *message << SB_SIGTYPE_PUB  << " "
               << current_status_;
      z_msg = new zmqpp::message();
      bool* more = new bool(true);

      while (*more) {
*more = false;
/*
        data_size += file->readFileData(contents,
                                         SB_MAXIMUM_FILE_PACKAGE_SIZE,
                                         offset,
                                         more);
        offset += SB_MAXIMUM_FILE_PACKAGE_SIZE;

        message->write(contents, data_size);
        *z_msg << message->str();
        z_dispatcher->send(*z_msg);
*/

        delete message;
        message = new std::stringstream();
        delete z_msg;
        z_msg = new zmqpp::message();
      }
    }

    delete sstream;
  }

  delete sstream;

  return 0;
}
