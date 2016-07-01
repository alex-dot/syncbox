/*
 * For each known syncbox node a subscriber subscribes to the corresponding
 * publisher. After receiving a message, a subscriber shall forward it
 * to the boxoffice. 
 * For each subscriber there shall be a separate subscriber thread. 
 */

#ifndef SB_SUBSCRIBER_HPP
#define SB_SUBSCRIBER_HPP

#include <zmqpp/zmqpp.hpp>

#include "transmitter.hpp"

class Subscriber : public Transmitter
{
  public:
    Subscriber() :
      Transmitter(), 
      z_subscriber(nullptr),
      endpoint(""),
      sb_subtype(-1)
      {};
    Subscriber(zmqpp::context* z_ctx_, std::string endpoint_, int sb_subtype_);
    Subscriber(const Subscriber&);
    ~Subscriber();

    int run();

  private:
    zmqpp::socket* z_subscriber;
    std::string endpoint;
    int sb_subtype;
};

#endif