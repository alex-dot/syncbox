/*
 * For each known syncbox node a subscriber subscribes to the corresponding
 * publisher. After receiving a message, a subscriber shall forward it
 * to the boxoffice. 
 * For each subscriber there shall be a separate subscriber thread. 
 */

#ifndef SB_SUBSCRIBER_HPP
#define SB_SUBSCRIBER_HPP

#include <zmqpp/zmqpp.hpp>

class Subscriber
{
  public:
    Subscriber() :
      z_ctx(nullptr),
      z_boxoffice_pull(nullptr),
      z_boxoffice_push(nullptr),
      z_subscriber(nullptr),
      z_broadcast(nullptr),
      endpoint(""),
      sb_subtype(-1)
      {};
    Subscriber(zmqpp::context* z_ctx_, std::string endpoint_, int sb_subtype_) :
      z_ctx(z_ctx_),
      z_boxoffice_pull(nullptr),
      z_boxoffice_push(nullptr),
      z_subscriber(nullptr),
      z_broadcast(nullptr),
      endpoint(endpoint_),
      sb_subtype(sb_subtype_)
      {};
    Subscriber(const Subscriber&);
    ~Subscriber();

    static Subscriber* initialize(zmqpp::context* z_ctx_, 
                                  std::string endpoint_, 
                                  int sb_subtype_);
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
    zmqpp::socket* z_subscriber;
    zmqpp::socket* z_broadcast;
    std::string endpoint;
    int sb_subtype;
};

#endif