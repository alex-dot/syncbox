/*
 * For each known syncbox node a subscriber subscribes to the corresponding
 * publisher. After receiving a message, a subscriber shall forward it
 * to the boxoffice. 
 * For each subscriber there shall be a separate subscriber thread. 
 */

#ifndef SB_SUBSCRIBER_HPP
#define SB_SUBSCRIBER_HPP

#include <zmq.hpp>

class Subscriber
{
  public:
    Subscriber() :
      z_ctx(nullptr),
      z_boxoffice(nullptr),
      z_subscriber(nullptr)
      {};
    Subscriber(const Subscriber&);
    ~Subscriber() {};

    static Subscriber* initialize(zmq::context_t* z_ctx_);
    int setContext(zmq::context_t* z_ctx_)
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
    int connectToBoxoffice();

    zmq::context_t* z_ctx;
    zmq::socket_t* z_boxoffice;
    zmq::socket_t* z_subscriber;
};

#endif