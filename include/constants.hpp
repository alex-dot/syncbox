/*
 * generic constants and globals for application
 */

#ifndef SB_CONSTANTS_HPP
#define SB_CONSTANTS_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <zmq.hpp>
#include <signal.h>

enum SB_SIGTYPE {
  SB_SIGTYPE_LIFE,
  SB_SIGTYPE_PUB,
  SB_SIGTYPE_SUB
};
enum SB_SIGLIFE {
  SB_SIGLIFE_ALIVE,
  SB_SIGLIFE_EXIT,
  SB_SIGLIFE_INTERRUPT,
  SB_SIGLIFE_ERROR=-1
};
enum SB_SIGPUB {
  SB_SIGPUB_GET_CHANNELS
};
enum SB_SIGSUB {
  SB_SIGSUB_GET_CHANNELS
};
enum SB_SUB_TYPE {
  SB_SUBTYPE_TCP_BIDIR,
  SB_SUBTYPE_TCP_UNIDIR
};

static bool SB_MSG_DEBUG = false;


static void s_recv(zmq::socket_t &socket, zmq::socket_t &broadcast, std::stringstream &sstream)
{
  zmq::message_t z_msg;
  zmq::pollitem_t z_items[] {
    { socket,    0, ZMQ_POLLIN, 0 },
    { broadcast, 0, ZMQ_POLLIN, 0 }
  };
  int more;
  while(true)
  {
    zmq::poll(&z_items[0], 2, -1);

    if ( z_items[0].revents & ZMQ_POLLIN )
    {
      while(true)
      {
        socket.recv(&z_msg);
        sstream << std::string(static_cast<char*>(z_msg.data()));
        size_t more_size = sizeof(more);
        socket.getsockopt(ZMQ_RCVMORE, &more, &more_size);
        if (!more)
          break;
      }
      break;
    }
    if ( z_items[1].revents & ZMQ_POLLIN )
    {
      broadcast.recv(&z_msg);
      sstream << std::string(static_cast<char*>(z_msg.data()));
      break;
    }
  }
}

#endif