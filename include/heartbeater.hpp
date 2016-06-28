/**
 * Each publisher has its own heartbeater thread that calls upon the 
 * publisher to send out heartbeats at semi-regular intervals. Each 
 * heartbeater shall be initialized and managed by the boxoffice. 
 * For each heartbeater there shall be a separate heartbeater thread. 
 */

#ifndef SB_HEARTBEATER_HPP
#define SB_HEARTBEATER_HPP

#include <zmqpp/zmqpp.hpp>

namespace fsm {
  #include "tarmuft_fsm.h"
}

class Heartbeater
{
  public:
    Heartbeater() :
      z_ctx(nullptr),
      z_boxoffice_pull(nullptr),
      z_boxoffice_push(nullptr),
      z_heartbeater(nullptr),
      z_broadcast(nullptr),
      current_status_(fsm::status_100)
      {};
    Heartbeater(zmqpp::context* z_ctx_, fsm::status_t status) :
      z_ctx(z_ctx_),
      z_boxoffice_pull(nullptr),
      z_boxoffice_push(nullptr),
      z_heartbeater(nullptr),
      z_broadcast(nullptr),
      current_status_(status)
      {};
    Heartbeater(const Heartbeater&);
    ~Heartbeater();

    static Heartbeater* initialize(zmqpp::context* z_ctx_, 
                                   fsm::status_t status);
    int setContext(zmqpp::context* z_ctx_)
    {
      z_ctx = z_ctx_;
      if ( z_ctx == nullptr )
        return 0;
      else
        return 1;

      return -1;
    }
    int sendExitSignal();
    int run();

  private:
    int connectToBroadcast();
    int connectToBoxoffice();

    zmqpp::context* z_ctx;
    zmqpp::socket* z_boxoffice_pull;
    zmqpp::socket* z_boxoffice_push;
    zmqpp::socket* z_heartbeater;
    zmqpp::socket* z_broadcast;
    fsm::status_t current_status_;
};

#endif