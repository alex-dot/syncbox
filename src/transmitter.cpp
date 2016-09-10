/*
 * Parentclass of all transmitting classes, i.e. subscribers, publishers, 
 * heartbeaters and boxes. 
 */

#include "constants.hpp"
#include "transmitter.hpp"

#include <unistd.h>

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread.hpp>

Transmitter::Transmitter() :
  z_ctx(nullptr),
  z_boxoffice_pull(nullptr),
  z_boxoffice_push(nullptr),
  z_broadcast(nullptr),
  tac((char*)"na") {
    this->connectToBroadcast();
    this->connectToBoxoffice();
}

Transmitter::Transmitter(zmqpp::context* z_ctx_) :
  z_ctx(z_ctx_),
  z_boxoffice_pull(nullptr),
  z_boxoffice_push(nullptr),
  z_broadcast(nullptr),
  tac((char*)"na") {
    this->connectToBroadcast();
    this->connectToBoxoffice();
}

Transmitter::~Transmitter()
{
  z_broadcast->close();
  z_boxoffice_pull->close();
  z_boxoffice_push->close();

  delete z_boxoffice_pull;
  delete z_boxoffice_push;
  delete z_broadcast;
}

int Transmitter::connectToBroadcast()
{
  // connect to process broadcast
  z_broadcast = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_broadcast->connect("inproc://f_broadcast");
  z_broadcast->subscribe("");

  return 0;
}

int Transmitter::connectToBoxoffice()
{
  // open connection to send data to boxoffice
  z_boxoffice_pull = new zmqpp::socket(*z_ctx, zmqpp::socket_type::push);
  z_boxoffice_pull->connect("inproc://f_boxoffice_pull_in");
  // open connection to receive data from boxoffice
  z_boxoffice_push = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_boxoffice_push->connect("inproc://f_boxoffice_push_out");
  z_boxoffice_push->subscribe("");

  // send a heartbeat to boxoffice, so it knows the heartbeater is ready
  if (F_MSG_DEBUG) printf("%s: sending heartbeat...\n", tac);
  std::stringstream message;
  message << F_SIGTYPE_LIFE << " " << F_SIGLIFE_ALIVE;
  zmqpp::message z_msg;
  z_msg << message.str();
  z_boxoffice_pull->send(z_msg);

  return 0;
}

int Transmitter::sendExitSignal()
{
  // send exit signal to boxoffice
  if (F_MSG_DEBUG) printf("%s: sending exit signal...\n", tac);
  std::stringstream message;
  message << F_SIGTYPE_LIFE << " " << F_SIGLIFE_EXIT;
  zmqpp::message z_msg;
  z_msg << message.str();
  z_boxoffice_pull->send(z_msg);

  if (F_MSG_DEBUG) printf("%s: signal sent, exiting...\n", tac);

  return 0;
}