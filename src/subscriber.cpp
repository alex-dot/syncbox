/*
 * Subscriber
 */

#include "constants.hpp"
#include "subscriber.hpp"

#include <unistd.h>

#include <zmq.hpp>
#include <string>
#include <sstream>
#include <iostream>

Subscriber* Subscriber::initialize(zmq::context_t* z_ctx_, 
                                   std::string endpoint_, 
                                   int sb_subtype_)
{
  Subscriber* sub = new Subscriber(z_ctx_, endpoint_, sb_subtype_);

  sub->connectToBoxoffice();

  return sub;
}

int Subscriber::connectToBoxoffice()
{
  z_boxoffice = new zmq::socket_t(*z_ctx, ZMQ_PUSH);
  z_boxoffice->connect("inproc://sb_boxoffice_subs");

  // send a heartbeat to boxoffice, so it knows the subscriber is ready
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_ALIVE);
  z_boxoffice->send(z_msg);
}

int Subscriber::sendExitSignal()
{
  // send exit signal to boxoffice
  std::cout << "sub: sending exit signal..." << std::endl;
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
  z_boxoffice->send(z_msg);

  std::cout << "sub: signal sent, exiting..." << std::endl;
  z_boxoffice->close();

  return 0;
}

int Subscriber::run()
{
  // internal check if subscriber was correctly initialized
  if ( z_ctx == nullptr || endpoint.compare("") == 0
      || ( sb_subtype != SB_SUBTYPE_TCP_BIDIR && sb_subtype != SB_SUBTYPE_TCP_UNIDIR ) )
    return 1;

  std::cout << "sub: receiving from publisher..." << std::endl;
  z_subscriber = new zmq::socket_t(*z_ctx, ZMQ_SUB);
  z_subscriber->connect(endpoint.c_str());
  const char *sub_filter = "";
  z_subscriber->setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);

  for (int i = 0; i < 3; ++i)
  {
    zmq::message_t z_message;
    z_subscriber->recv(&z_message);

    std::istringstream iss(static_cast<char*>(z_message.data()));
    std::cout << "sub:" << iss.str() << std::endl;
  }

  return 0;
}