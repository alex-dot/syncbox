/*
 * Subscriber
 */

#include "constants.hpp"
#include "subscriber.hpp"

#include <unistd.h>

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread.hpp>


Subscriber::~Subscriber()
{
  delete z_boxoffice_pull;
  delete z_boxoffice_push;
  delete z_subscriber;
  delete z_broadcast;
}

Subscriber* Subscriber::initialize(zmqpp::context* z_ctx_, 
                                   std::string endpoint_, 
                                   int sb_subtype_)
{
  Subscriber* sub = new Subscriber(z_ctx_, endpoint_, sb_subtype_);

  sub->connectToBroadcast();
  sub->connectToBoxoffice();

  return sub;
}

int Subscriber::connectToBroadcast()
{
  // connect to process broadcast
  // since we want to rarely use that (mostly interrupt) we assume that by 
  // the time we need it, we have long been connected to it
  z_broadcast = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_broadcast->connect("inproc://sb_broadcast");
  z_broadcast->subscribe("");

  return 0;
}

int Subscriber::connectToBoxoffice()
{
  // open connection to send data to boxoffice
  z_boxoffice_pull = new zmqpp::socket(*z_ctx, zmqpp::socket_type::push);
  z_boxoffice_pull->connect("inproc://sb_boxoffice_pull_in");
  // open connection to receive data from boxoffice
  z_boxoffice_push = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_boxoffice_push->connect("inproc://sb_boxoffice_push_out");
  z_boxoffice_push->subscribe("");

  // send a heartbeat to boxoffice, so it knows the publisher is ready
  if (SB_MSG_DEBUG) printf("pub: sending heartbeat...\n");
  std::stringstream message;
  message << SB_SIGTYPE_LIFE << " " << SB_SIGLIFE_ALIVE;
  zmqpp::message z_msg;
  z_msg << message.str();
  z_boxoffice_pull->send(z_msg);

  return 0;
}

int Subscriber::sendExitSignal()
{
  // send exit signal to boxoffice
  if (SB_MSG_DEBUG) printf("sub: sending exit signal...\n");
  std::stringstream message;
  message << SB_SIGTYPE_LIFE << " " << SB_SIGLIFE_EXIT;
  zmqpp::message z_msg;
  z_msg << message.str();
  z_boxoffice_pull->send(z_msg);

  if (SB_MSG_DEBUG) printf("sub: signal sent, exiting...\n");
  z_boxoffice_pull->close();
  z_boxoffice_push->close();
  z_subscriber->close();
  z_broadcast->close();

  return 0;
}

int Subscriber::run()
{
  // internal check if subscriber was correctly initialized
  if ( z_ctx == nullptr || endpoint.compare("") == 0
      || ( sb_subtype != SB_SUBTYPE_TCP_BIDIR && sb_subtype != SB_SUBTYPE_TCP_UNIDIR ) )
    return 1;

  if (SB_MSG_DEBUG) printf("sub: receiving from publisher...\n");
  z_subscriber = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_subscriber->connect(endpoint.c_str());
  z_subscriber->subscribe("");

  std::stringstream* sstream;
  int msg_type, msg_signal;
  std::string infomessage;

  while (true)
  {
    sstream = new std::stringstream();
    s_recv(*z_boxoffice_push, *z_broadcast, *z_subscriber, *sstream);
    *sstream >> msg_type;
    if ( msg_type == SB_SIGTYPE_LIFE ) {
      *sstream >> msg_signal;
      if ( msg_signal == SB_SIGLIFE_INTERRUPT ) break;
    } else {
      msg_signal = msg_type;
    }

    *sstream >> infomessage;
    std::stringstream message;
    message << SB_SIGTYPE_SUB << " "
            << msg_signal     << " "
            << infomessage.c_str();
    zmqpp::message z_msg;
    z_msg << message.str();
    z_boxoffice_pull->send(z_msg);

    delete sstream;
  }
  delete sstream;

  return 0;
}