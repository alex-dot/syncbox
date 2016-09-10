/**
 * Each configured access point shall have its own publisher to send 
 * its configured box data across. Every publisher binds sockets where it 
 * publishes box state changes for an arbitrary number of (external) 
 * subscribers to receive. It shall react on messages from the boxoffice 
 * and send the appropriate messages accordingly. 
 * For each publisher there shall be a separate publisher thread. 
 */

#ifndef F_PUBLISHER_HPP
#define F_PUBLISHER_HPP

#include <zmqpp/zmqpp.hpp>

#include "transmitter.hpp"
#include "config.hpp"

class Publisher : public Transmitter
{
  public:
    Publisher() :
      Transmitter(), 
      z_heartbeater(nullptr),
      z_dispatcher(nullptr),
      z_publisher(nullptr),
      data()
      {};
    Publisher(zmqpp::context* z_ctx_, host_t data_);
    Publisher(const Publisher&);
    ~Publisher();

    int run();

  private:
    int connectToHeartbeater();
    int connectToDispatcher();

    zmqpp::socket* z_heartbeater;
    zmqpp::socket* z_dispatcher;
    zmqpp::socket* z_publisher;
    host_t         data;
};

#endif