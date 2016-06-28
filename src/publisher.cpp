/*
 * Publisher
 */

#include "constants.hpp"
#include "publisher.hpp"

#include <unistd.h>

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread.hpp>


Publisher::~Publisher()
{
  delete z_boxoffice_pull;
  delete z_boxoffice_push;
  delete z_publisher;
  delete z_heartbeater;
  delete z_broadcast;
}

Publisher* Publisher::initialize(zmqpp::context* z_ctx_, 
                                 std::string endpoint_)
{
  Publisher* pub = new Publisher(z_ctx_, endpoint_);

  pub->connectToBroadcast();
  pub->connectToBoxoffice();
  pub->connectToHeartbeater();

  return pub;
}

int Publisher::connectToBroadcast()
{
  // connect to process broadcast
  z_broadcast = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_broadcast->connect("inproc://sb_broadcast");
  z_broadcast->subscribe("");

  return 0;
}

int Publisher::connectToBoxoffice()
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

int Publisher::connectToHeartbeater()
{
  // connect to process broadcast
  z_heartbeater = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pair);
  z_heartbeater->connect("inproc://pub_hb_pair");

  return 0;
}

int Publisher::sendExitSignal()
{
  // send exit signal to boxoffice
  if (SB_MSG_DEBUG) printf("pub: sending exit signal...\n");
  std::stringstream message;
  message << SB_SIGTYPE_LIFE << " " << SB_SIGLIFE_EXIT;
  zmqpp::message z_msg;
  z_msg << message.str();
  z_boxoffice_pull->send(z_msg);

  if (SB_MSG_DEBUG) printf("pub: signal sent, exiting...\n");

  z_publisher->close();
  z_heartbeater->close();
  z_broadcast->close();
  z_boxoffice_pull->close();
  z_boxoffice_push->close();

  return 0;
}

int Publisher::run()
{
  // internal check if publisher was correctly initialized
  if ( z_ctx == nullptr || endpoint.compare("") == 0 )
    return 1;

  if (SB_MSG_DEBUG) printf("pub: starting pub socket and sending...\n");
  z_publisher = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pub);
  z_publisher->bind(endpoint.c_str());

  std::stringstream* sstream;
  int msg_type, msg_signal;

  while(true)
  {
    // waiting for boxoffice input in non-blocking mode
    sstream = new std::stringstream();
    s_recv(*z_boxoffice_push, *z_broadcast, *z_heartbeater, *sstream);

    *sstream >> msg_type >> msg_signal;
    if ( msg_type == SB_SIGTYPE_LIFE && msg_signal == SB_SIGLIFE_INTERRUPT ) break;

    // send a message
    std::string infomessage;
    *sstream >> infomessage;
    if (SB_MSG_DEBUG) printf("pub: %d %s\n", msg_signal, infomessage.c_str());
    std::stringstream message;
    message << msg_signal << " " << infomessage.c_str();
    zmqpp::message z_msg;
    z_msg << message.str();
    z_publisher->send(z_msg);

    delete sstream;
  }

  delete sstream;

  return 0;
}