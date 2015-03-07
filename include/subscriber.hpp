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
      z_subscriber(nullptr),
      z_broadcast(nullptr),
      endpoint(""),
      sb_subtype(-1)
      {};
    Subscriber(zmq::context_t* z_ctx_, std::string endpoint_, int sb_subtype_) :
      z_ctx(z_ctx_),
      z_boxoffice(nullptr),
      z_subscriber(nullptr),
      z_broadcast(nullptr),
      endpoint(endpoint_),
      sb_subtype(sb_subtype_)
      {};
    Subscriber(const Subscriber&);
    ~Subscriber();

    static Subscriber* initialize(zmq::context_t* z_ctx_, 
                                  std::string endpoint_, 
                                  int sb_subtype_);
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
    int connectToBroadcast();
    int connectToBoxoffice();

    zmq::context_t* z_ctx;
    zmq::socket_t* z_boxoffice;
    zmq::socket_t* z_subscriber;
    zmq::socket_t* z_broadcast;
    std::string endpoint;
    int sb_subtype;
};

#endif