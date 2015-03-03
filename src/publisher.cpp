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
#include <boost/thread.hpp>

Publisher* Publisher::initialize(zmq::context_t* z_ctx_)
{
  Publisher* pub = getInstance();

  pub->setContext(z_ctx_);
  pub->connectToBroadcast();
  pub->connectToBoxoffice();

  return pub;
}

int Publisher::connectToBroadcast()
{
  // connect to process broadcast
  z_broadcast = new zmq::socket_t(*z_ctx, ZMQ_SUB);
  z_broadcast->connect("inproc://sb_broadcast");
  const char *sub_filter = "";
  z_broadcast->setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);

  return 0;
}

int Publisher::connectToBoxoffice()
{
  // since the publisher is a singleton, we can simply use two ZMQ_PAIRs
  z_pub_pair = new zmq::socket_t(*z_ctx, ZMQ_PAIR);
  z_pub_pair->bind("inproc://sb_pub_bo_pair");

  // send a heartbeat to boxoffice, so it knows the publisher is ready
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_ALIVE);
  z_pub_pair->send(z_msg);

  return 0;
}

int Publisher::sendExitSignal()
{
  // send exit signal to boxoffice
  if (SB_MSG_DEBUG) printf("pub: sending exit signal\n");
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
  z_pub_pair->send(z_msg);

  z_pub_pair->close();

  z_broadcast->close();

  if (SB_MSG_DEBUG) printf("pub: exit signal sent, exiting...\n");

  return 0;
}

int Publisher::run()
{
  zmq::message_t z_msg;
  int msg_type, msg_signal;
  std::stringstream* sstream;

  // query pub channels from boxoffice
  if (SB_MSG_DEBUG) printf("pub: querying channels.\n");
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_PUB, SB_SIGPUB_GET_CHANNELS);
  z_pub_pair->send(z_msg);

  // receiving channels and starting them
  if (SB_MSG_DEBUG) printf("pub: getting channels.\n");
  while(true)
  {
    sstream = new std::stringstream();
    s_recv(*z_pub_pair, *z_broadcast, *sstream);
    if (*sstream >> msg_type >> msg_signal) {
      if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_ALIVE ) { return 1; }
        else { return -1; } }
    std::string channel_name = sstream->str();
    delete sstream;
    if (SB_MSG_DEBUG) printf("pub: channel name: %s\n", channel_name.c_str());

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
  if (SB_MSG_DEBUG) printf("pub: starting pubsub sockets and sending...\n");

  zmq::socket_t* z_publisher = new zmq::socket_t(*z_ctx, ZMQ_PUB);
  z_publisher->bind(channel_name.c_str());
  channel_list.push_back(z_publisher);

  return 0;
}

int Publisher::stopPubChannel(zmq::socket_t* channel)
  { channel->close(); return 0; }

int Publisher::listenOnChannels()
{
  zmq::socket_t* channel = channel_list.back();
  for (int i = 0; i < 2; ++i)
  {
    if (boost::this_thread::interruption_requested()) 
      break;

    std::string message = "this is message: " + std::to_string(i);
    if (SB_MSG_DEBUG) printf("pub: %s\n", message.c_str());
    zmq::message_t z_msg(message.length()+1);
    snprintf((char*) z_msg.data(), message.length()+1, "%s", message.c_str());
    channel->send(z_msg, ZMQ_SNDMORE);
    channel->send(z_msg, ZMQ_SNDMORE);
    channel->send(z_msg);
    sleep(1);
  }
  return 0;
}