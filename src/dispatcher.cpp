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
#include <iomanip>
#include <boost/thread.hpp>
#include <chrono>
#include <thread>

Dispatcher::Dispatcher(zmqpp::context* z_ctx_, fsm::status_t status) :
  Transmitter(z_ctx_),
  z_dispatcher(nullptr),
  z_boxoffice_disp_push(nullptr),
  current_status_(status),
  timing_offset_(-1),
  waiting_for_stop_(false) {
    tac = (char*)"dis";
    this->connectToBoxofficeDispatcher();
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

int Dispatcher::connectToBoxofficeDispatcher()
{
  // open connection to receive HB data from boxoffice
  z_boxoffice_disp_push = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_boxoffice_disp_push->connect("inproc://f_boxoffice_disp_push_out");
  z_boxoffice_disp_push->subscribe("");

  return 0;
}

int Dispatcher::run()
{
  // internal check if heartbeater was correctly initialized
  if ( z_ctx == nullptr )
    return 1;

  if (F_MSG_DEBUG) printf("dis: starting disp socket and sending...\n");

  std::stringstream* sstream;
  int msg_type, msg_signal;

  while(true)
  {
    // waiting for boxoffice input in non-blocking mode
    sstream = new std::stringstream();
    s_recv(*z_boxoffice_push, *z_boxoffice_disp_push, *z_broadcast, *sstream);

    *sstream >> msg_type >> msg_signal;
    if ( msg_type == F_SIGTYPE_LIFE && msg_signal == F_SIGLIFE_INTERRUPT ) break;
    if ( msg_type == F_SIGTYPE_FSM ) {
      current_status_ = (fsm::status_t)msg_signal;
    }

    if (F_MSG_DEBUG) printf("dis: sending file data status %d\n", (int)current_status_);

    if ( current_status_ == fsm::status_122
      || current_status_ == fsm::status_177 ) {
      uint64_t timing_offset;
      char* timing_offset_c = new char[8];
      sstream->seekg(1, std::ios_base::cur);
      sstream->read(timing_offset_c, 8);
      std::memcpy(&timing_offset, timing_offset_c, 8);
      uint64_t timestamp =
        std::chrono::duration_cast< std::chrono::milliseconds >(
          std::chrono::system_clock::now().time_since_epoch()
        ).count();
      timing_offset_ = be64toh(timing_offset) - timestamp;

// \TODO needs individual offset
      std::this_thread::sleep_for(std::chrono::milliseconds(timing_offset_));
      if (F_MSG_DEBUG) printf("dis: time's up!\n");

      std::stringstream* message = new std::stringstream();
      *message << F_SIGTYPE_FSM  << " "
               << fsm::status_132;
      zmqpp::message* z_msg = new zmqpp::message();
      *z_msg << message->str();
      z_boxoffice_pull->send(*z_msg);
      delete message;
      message = new std::stringstream();
      delete z_msg;
      z_msg = new zmqpp::message();

      std::this_thread::sleep_for(std::chrono::milliseconds(250));

      current_status_ = fsm::status_200;

      char box_hash_s[F_GENERIC_HASH_LEN];
      sstream->read(box_hash_s, F_GENERIC_HASH_LEN);
      unsigned char box_hash[F_GENERIC_HASH_LEN];
      std::memcpy(box_hash, box_hash_s, F_GENERIC_HASH_LEN);
      Hash* hash = new Hash(box_hash);
      std::string box_dir;
      *sstream >> box_dir;

      File* file = new File(box_dir, hash);
      sstream->seekg(1, std::ios_base::cur);
      *sstream >> *file;

      char* contents = new char[F_MAXIMUM_FILE_PACKAGE_SIZE];
      bool* more = new bool(true);
      uint64_t data_size = 0;
      uint64_t offset = 0;

      message = new std::stringstream();
      *message << F_SIGTYPE_PUB  << " "
               << std::to_string(current_status_);
      z_msg = new zmqpp::message();

      while (*more) {
        data_size += file->readFileData(contents,
                                        F_MAXIMUM_FILE_PACKAGE_SIZE,
                                        offset,
                                        more);

        uint64_t offset_be = htobe64(offset);
        char* offset_c = new char[8];
        std::memcpy(offset_c, &offset_be, 8);
        message->write(offset_c, 8);

        offset += F_MAXIMUM_FILE_PACKAGE_SIZE;

        uint64_t data_size_be = htobe64(data_size);
        char* data_size_c = new char[8];
        std::memcpy(data_size_c, &data_size_be, 8);
        message->write(data_size_c, 8);

        int8_t more_i = static_cast<int8_t>(*more);
        char* more_c = new char[1];
        std::memcpy(more_c, &more_i, 1);
        message->write(more_c, 1);

        message->write(contents, data_size);

        int p = message->tellp();
        if (F_MAXIMUM_FILE_PACKAGE_SIZE+22-p > 0)
          *message << std::setw(F_MAXIMUM_FILE_PACKAGE_SIZE+22-p)
                   << std::setfill(' ') << " ";

        *z_msg << message->str();
        z_dispatcher->send(*z_msg);

        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        delete contents;
        contents = new char[F_MAXIMUM_FILE_PACKAGE_SIZE];
        delete message;
        message = new std::stringstream();
        delete z_msg;
        z_msg = new zmqpp::message();
      }

      delete message;
      delete z_msg;
      delete contents;

      current_status_ = fsm::status_210;
      int retval = synchronizingStop();
      if (retval == 0) return retval;

    } else if (current_status_ == fsm::status_130) {
      if (F_MSG_DEBUG) printf("dis: sending fake file data status %d\n", (int)current_status_);
      uint64_t timing_offset;
      char* timing_offset_c = new char[8];
      sstream->seekg(1, std::ios_base::cur);
      sstream->read(timing_offset_c, 8);
      std::memcpy(&timing_offset, timing_offset_c, 8);
      uint64_t timestamp =
        std::chrono::duration_cast< std::chrono::milliseconds >(
          std::chrono::system_clock::now().time_since_epoch()
        ).count();
      timing_offset_ = be64toh(timing_offset) - timestamp;

// \TODO needs individual offset
      std::this_thread::sleep_for(std::chrono::milliseconds(timing_offset_));
      if (F_MSG_DEBUG) printf("dis: time's up!\n");

      std::stringstream* message = new std::stringstream();
      *message << F_SIGTYPE_FSM  << " "
               << std::to_string(fsm::status_132);
      zmqpp::message* z_msg = new zmqpp::message();
      *z_msg << message->str();
      z_boxoffice_pull->send(*z_msg);
      delete message;
      delete z_msg;

      std::this_thread::sleep_for(std::chrono::milliseconds(250));

      current_status_ = fsm::status_210;
      int retval = synchronizingStop();
      if (retval == 0) return retval;
    }

  }

  delete sstream;

  return 0;
}

int Dispatcher::synchronizingStop() {
  while (true) {
    std::stringstream* sstream = new std::stringstream();
    int msg_type, msg_signal;
    int z_return = s_recv_noblock(*z_boxoffice_push,
                                  *z_boxoffice_disp_push,
                                  *z_broadcast,
                                  *sstream,
                                  250);
    if ( z_return != 0 ) {
      *sstream >> msg_type >> msg_signal;
      if ( msg_type == F_SIGTYPE_LIFE && msg_signal == F_SIGLIFE_INTERRUPT ) {
        delete sstream;
        return 0;
      }
      if ( msg_type == F_SIGTYPE_FSM  && msg_signal == fsm::status_155 ) {
        uint64_t timing_offset;
        char* timing_offset_c = new char[8];
        sstream->seekg(1, std::ios_base::cur);
        sstream->read(timing_offset_c, 8);
        std::memcpy(&timing_offset, timing_offset_c, 8);
        timing_offset_ = be64toh(timing_offset);
        waiting_for_stop_ = true;
      }
    } else {
      uint64_t timestamp =
        std::chrono::duration_cast< std::chrono::milliseconds >(
          std::chrono::system_clock::now().time_since_epoch()
        ).count();
      if (waiting_for_stop_ && timestamp - timing_offset_ <= 1000000) {
        if (F_MSG_DEBUG) printf("dis: stopping transmission\n");
        break;
      }
    }

    delete sstream;
    sendFakeData();
  }

  std::stringstream* message = new std::stringstream();
  *message << F_SIGTYPE_FSM  << " "
           << std::to_string(fsm::status_156);
  zmqpp::message* z_msg = new zmqpp::message();
  *z_msg << message->str();
      z_boxoffice_pull->send(*z_msg);
  return 1;
}

void Dispatcher::sendFakeData() const {
  zmqpp::message* z_msg = new zmqpp::message();
  std::stringstream* message = new std::stringstream();
  *message << F_SIGTYPE_PUB  << " "
           << current_status_;

  int p = message->tellp();
  *message << std::setw(F_MAXIMUM_FILE_PACKAGE_SIZE+22-p)
           << std::setfill(' ') << " ";

  *z_msg << message->str();
  z_dispatcher->send(*z_msg);

  delete message;
  delete z_msg;
}
