/**
 * Each configured access point shall have its own publisher to send 
 * its configured box data across. Every publisher binds sockets where it 
 * publishes box state changes for an arbitrary number of (external) 
 * subscribers to receive. It shall react on messages from the boxoffice 
 * and send the appropriate messages accordingly. 
 * For each publisher there shall be a separate publisher thread. 
 */

#ifndef SB_PUBLISHER_HPP
#define SB_PUBLISHER_HPP

#include <zmqpp/zmqpp.hpp>

#include "transmitter.hpp"

class Publisher : public Transmitter
{
  public:
    Publisher() :
      Transmitter(), 
      z_heartbeater(nullptr),
      z_publisher(nullptr),
      endpoint()
      {};
    Publisher(zmqpp::context* z_ctx_, std::string endpoint_);
    Publisher(const Publisher&);
    ~Publisher();

    int run();

  private:
    int connectToHeartbeater();

    zmqpp::socket* z_heartbeater;
    zmqpp::socket* z_publisher;
    std::string endpoint;
};

#endif