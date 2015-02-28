/*
 * Publisher
 */

#include "constants.hpp"
#include "publisher.hpp"

#include <unistd.h>

#include <zmq.hpp>
#include <string>
#include <sstream>
#include <iostream>

Publisher* Publisher::initialize(zmq::context_t* z_ctx_)
{
  Publisher* pub = getInstance();

  pub->setContext(z_ctx_);
  pub->connectToBoxoffice();

  return pub;
}

int Publisher::connectToBoxoffice()
{
  // since the publisher is a singleton, we can simply use two ZMQ_PAIRs
  z_pub_to_bo = new zmq::socket_t(*z_ctx, ZMQ_PAIR);
  z_bo_to_pub = new zmq::socket_t(*z_ctx, ZMQ_PAIR);
  z_pub_to_bo->connect("inproc://sb_pub_to_bo_pair");
  z_bo_to_pub->bind("inproc://sb_bo_to_pub_pair");

  // send a heartbeat to boxoffice, so it knows the publisher is ready
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_ALIVE);
  z_pub_to_bo->send(z_msg);

  return 0;
}

int Publisher::sendExitSignal()
{
  // send exit signal to boxoffice
  std::cout << "pub: sending exit signal" << std::endl;
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
  z_pub_to_bo->send(z_msg);

  z_pub_to_bo->close();

  std::cout << "pub: exit signal sent, exiting..." << std::endl;

  return 0;
}

int Publisher::run()
{
  zmq::message_t z_msg;
  int msg_type, msg_signal;
  std::stringstream sstream;

  // query pub channels from boxoffice
  std::cout << "pub: querying channels." << std::endl;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_PUB, SB_SIGPUB_GET_CHANNELS);
  z_pub_to_bo->send(z_msg);

  // receiving channels and starting them
  std::cout << "pub: getting channels." << std::endl;
  while(true)
  {
    std::string channel_name;
    z_pub_to_bo->recv(&z_msg, 0);
    sstream.clear();
    sstream << static_cast<char*>(z_msg.data());
    sstream >> channel_name;
    std::cout << "pub: channel name: " << channel_name.c_str() << std::endl;

    startPubChannel(channel_name);

    if ( !z_msg.more() ) break;
  }

  listenOnChannels();

  // closing channels
  for (std::vector<zmq::socket_t*>::iterator i = channel_list.begin(); i != channel_list.end(); ++i)
    stopPubChannel(*i);

  return 0;
}

int Publisher::startPubChannel(std::string channel_name)
{
  std::cout << "pub: starting pubsub sockets and sending..." << std::endl;

  zmq::socket_t* z_publisher = new zmq::socket_t(*z_ctx, ZMQ_PUB);
  z_publisher->bind(channel_name.c_str());
  channel_list.push_back(z_publisher);

  return 0;
}

int Publisher::stopPubChannel(zmq::socket_t* channel)
  { channel->close(); }

int Publisher::listenOnChannels()
{
  zmq::socket_t* channel = channel_list.back();
  for (int i = 0; i < 5; ++i)
  {
    std::string message = "this is message: " + std::to_string(i);
    std::cout << "pub: " << message << std::endl;
    zmq::message_t z_msg(message.length()+1);
    snprintf((char*) z_msg.data(), message.length()+1, "%s", message.c_str());
    channel->send(z_msg);
    sleep(1);
  }
}