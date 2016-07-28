/**
 * Each publisher has its own dispatcher thread that calls upon the 
 * publisher to send out the data of a file once a filedata transfer 
 * has been scheduled. At the time of transmission, it shall send
 * packages of data as specified in the protocol to the publisher. Each 
 * dispatcher shall be initialized and managed by the boxoffice. 
 * For each dispatcher there shall be a separate dispatcher thread. 
 */

#ifndef SB_DISPATCHER_HPP
#define SB_DISPATCHER_HPP

#include <zmqpp/zmqpp.hpp>

#include "transmitter.hpp"

namespace fsm {
  #include "tarmuft_fsm.h"
}

class Dispatcher : public Transmitter
{
  public:
    Dispatcher() :
      Transmitter(),
      z_dispatcher(nullptr),
      z_boxoffice_disp_push(nullptr),
      current_status_(fsm::status_100),
      timing_offset_(-1)
      {};
    Dispatcher(zmqpp::context* z_ctx_, fsm::status_t status);
    Dispatcher(const Dispatcher&);
    ~Dispatcher();

    int run();

  private:
    int connectToPublisher();
    int connectToBoxofficeHB();

    zmqpp::socket* z_dispatcher;
    zmqpp::socket* z_boxoffice_disp_push;
    fsm::status_t  current_status_;
    uint64_t       timing_offset_;
};

#endif