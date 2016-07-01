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
#include "config.hpp"

class Subscriber : public Transmitter
{
  public:
    Subscriber() :
      Transmitter(), 
      z_subscriber(nullptr),
      data()
      {};
    Subscriber(zmqpp::context* z_ctx_, node_t data);
    Subscriber(const Subscriber&);
    ~Subscriber();

    int run();

  private:
    zmqpp::socket* z_subscriber;
    node_t         data;
};

#endif