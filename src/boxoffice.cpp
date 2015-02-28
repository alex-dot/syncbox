/*
 * Boxoffice
 */

#include "constants.hpp"
#include "boxoffice.hpp"

#include <zmq.hpp>
#include <sstream>
#include <iostream>

Boxoffice* Boxoffice::initialize(zmq::context_t* z_ctx)
{
  Boxoffice* bo = getInstance();

  // setting up
  bo->setContext(z_ctx);
  bo->connectToMain();
  bo->connectToPub();

  // setting up the router and listen to all subscriber threads
  // note that the Watchers are all subscribers too!
  bo->runRouter();

  // closing
  bo->closeConnections();

  return bo;
}

int Boxoffice::connectToMain()
{
  // open PAIR sender to main thread
  z_bo_main = new zmq::socket_t(*z_ctx, ZMQ_PAIR);
  z_bo_main->connect("inproc://sb_bo_main_pair");
}

int Boxoffice::connectToPub()
{
  // standard variables
  zmq::message_t z_msg;
  int msg_type, msg_signal;
  std::stringstream sstream;

  // since the publisher is a singleton, we can simply use a ZMQ_PAIR
  z_pub_to_bo = new zmq::socket_t(*z_ctx, ZMQ_PAIR);
  z_pub_to_bo->bind("inproc://sb_pub_to_bo_pair");

  // wait for heartbeat from publisher
  std::cout << "bo: waiting for publisher to send heartbeat" << std::endl;
  z_pub_to_bo->recv(&z_msg);
  sstream.clear();
  sstream << static_cast<char*>(z_msg.data());
  sstream >> msg_type >> msg_signal;
  if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_ALIVE ) return 1;

  // sending publisher channel list
  std::cout << "bo: waiting to send publisher channel list" << std::endl;
  z_pub_to_bo->recv(&z_msg);
  sstream.clear();
  sstream << static_cast<char*>(z_msg.data());
  sstream >> msg_type >> msg_signal;
  if ( msg_type != SB_SIGTYPE_PUB || msg_signal != SB_SIGPUB_GET_CHANNELS ) return 1;
  else {
    std::cout << "bo: sending publisher channel list" << std::endl;
    std::vector<std::string> channel_list;
    channel_list.push_back("ipc://syncbox.ipc");
    for (std::vector<std::string>::iterator i = channel_list.begin(); i != channel_list.end(); ++i)
    {
      memcpy(z_msg.data(), i->data(), i->size()+1);
      if ( (*i) != channel_list.back() )
        z_pub_to_bo->send(z_msg, ZMQ_SNDMORE);
      else
        z_pub_to_bo->send(z_msg, 0); 
    }
  }
}

int Boxoffice::runRouter()
{
  // standard variables
  zmq::message_t z_msg;
  int msg_type, msg_signal;
  std::stringstream sstream;

  // connect to subscribers
  z_router = new zmq::socket_t(*z_ctx, ZMQ_ROUTER);
  z_router->bind("inproc://sb_boxoffice_req");

  std::cout << "bo: starting to listen to subs..." << std::endl;

  zmq::pollitem_t z_items[] = {
    { *z_router, 0, ZMQ_POLLIN, 0 }
  };

  bool exit_signal = false;
  while(true) {
    int more;

    zmq::poll(&z_items[0], 1, -1);

    if (z_items[0].revents & ZMQ_POLLIN) {
      while(true) {
        int msg_type, msg_signal;
        zmq::message_t z_msg;

        std::cout << "bo: waiting for subscriber to send exit signal" << std::endl;
        z_router->recv(&z_msg);
        size_t more_size = sizeof(more);
        z_router->getsockopt(ZMQ_RCVMORE, &more, &more_size);

        std::istringstream iss_sub(static_cast<char*>(z_msg.data()));
        iss_sub >> msg_type >> msg_signal;
        std::cout << "bo: sub sent: " << msg_type << " " << msg_signal << std::endl;
        if ( msg_type == SB_SIGTYPE_LIFE && msg_signal == SB_SIGLIFE_EXIT )
        {
          snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
          z_router->send(z_msg, more? ZMQ_SNDMORE: 0);
          exit_signal = true;
          break;
        } else {
          snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_ERROR);
          z_router->send(z_msg);
        }
      }
    }
    if (exit_signal) break;
  }

  // wait for exit signal from publisher
  std::cout << "bo: waiting for publisher to send exit signal" << std::endl;
  z_pub_to_bo->recv(&z_msg);
  sstream.clear();
  sstream << static_cast<char*>(z_msg.data());
  sstream >> msg_type >> msg_signal;

  return 0;
}

int Boxoffice::closeConnections()
{
  z_pub_to_bo->close();

  std::cout << "bo: sending exit signal..." << std::endl;
  zmq::message_t z_msg_close(3);
  snprintf((char*) z_msg_close.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
  z_bo_main->send(z_msg_close);

  z_bo_main->close();

  std::cout << "bo: exit signal sent, exiting..." << std::endl;
}