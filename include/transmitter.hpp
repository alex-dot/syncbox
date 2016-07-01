/**
 * Parentclass of all transmitting classes, i.e. subscribers, publishers, 
 * heartbeaters and boxes. 
 */

#ifndef SB_TRANSMITTER_HPP
#define SB_TRANSMITTER_HPP

#include <zmqpp/zmqpp.hpp>

namespace fsm {
  #include "tarmuft_fsm.h"
}

class Transmitter
{
  public:
    Transmitter();
    Transmitter(zmqpp::context* z_ctx_);
    Transmitter(const Transmitter&);
    virtual ~Transmitter();

    virtual int setContext(zmqpp::context* z_ctx_)
    {
      z_ctx = z_ctx_;
      if ( z_ctx == nullptr )
        return 0;
      else
        return 1;

      return -1;
    }
    virtual int sendExitSignal();
    virtual int run() = 0;

  protected:
    zmqpp::context* z_ctx;
    zmqpp::socket* z_boxoffice_pull;
    zmqpp::socket* z_boxoffice_push;
    zmqpp::socket* z_broadcast;
    char* tac; // Transmitter ACronym

  private:
    int connectToBroadcast();
    int connectToBoxoffice();
};

#endif