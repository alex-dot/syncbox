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

static bool SB_MSG_DEBUG = true;


static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals ()
{
#if (!defined(WIN32))
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
#endif
}

static std::string s_recv(zmq::socket_t &socket, zmq::socket_t &broadcast)
{
  zmq::message_t z_msg;
  zmq::pollitem_t z_items[] {
    { socket,    0, ZMQ_POLLIN, 0 },
    { broadcast, 0, ZMQ_POLLIN, 0 }
  };
  while(true)
  {
    zmq::poll(&z_items[0], 2, -1);

    if ( z_items[0].revents & ZMQ_POLLIN )
    {
      socket.recv(&z_msg);
      break;
    }
    if ( z_items[1].revents & ZMQ_POLLIN )
    {
      // since broadcast is a publisher, we may receive garbage upfront
      //std::stringstream sstream;
      //int msg_type = -1;
      //int msg_signal;
      broadcast.recv(&z_msg);
      //sstream << static_cast<char*>(z_msg.data());
      //sstream >> msg_type >> msg_signal;
      //std::cout << msg_type << " " << msg_signal << std::endl;
      //if ( msg_type == SB_SIGTYPE_LIFE )
        break;
    }
  }

  return static_cast<char*>(z_msg.data());
}

#endif