/**
 * Each publisher has its own heartbeater thread that calls upon the 
 * publisher to send out heartbeats at semi-regular intervals. Each 
 * heartbeater shall be initialized and managed by the boxoffice. 
 * For each heartbeater there shall be a separate heartbeater thread. 
 */

#ifndef SB_HEARTBEATER_HPP
#define SB_HEARTBEATER_HPP

#include <zmqpp/zmqpp.hpp>

#include "transmitter.hpp"

namespace fsm {
  #include "tarmuft_fsm.h"
}

class Heartbeater : public Transmitter
{
  public:
    Heartbeater() :
      Transmitter(),
      z_heartbeater(nullptr), 
      current_status_(fsm::status_100),
      current_message_("")
      {};
    Heartbeater(zmqpp::context* z_ctx_, fsm::status_t status);
    Heartbeater(const Heartbeater&);
    ~Heartbeater();

    int run();

  private:
    int connectToPublisher();

    zmqpp::socket* z_heartbeater;
    fsm::status_t current_status_;
    std::string   current_message_;
};

#endif