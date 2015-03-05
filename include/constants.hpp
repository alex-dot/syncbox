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
#include <unistd.h>
#include <sys/inotify.h>

enum SB_SIGTYPE {
  SB_SIGTYPE_LIFE,
  SB_SIGTYPE_PUB,
  SB_SIGTYPE_SUB,
  SB_SIGTYPE_INOTIFY
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
enum SB_SIGIN {
  SB_SIGIN_EVENT
};

#define SB_MSG_DEBUG false

//static size_t  
#define SB_IN_EVENT_SIZE (sizeof(struct inotify_event))
//static size_t  
#define SB_IN_BUF_LEN    (1024 * (SB_IN_EVENT_SIZE + 16))
#define SB_IN_EVENT_MASK  IN_ATTRIB|IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_MODIFY|IN_MOVE|IN_MOVE_SELF


// wrapper for polling on one socket while simultaneously polling the broadcast
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

// wrapper for polling on inotify event while simultaneously polling the broadcast
static void s_recv_in(zmq::socket_t &broadcast, int fd, std::stringstream &sstream)
{
  zmq::message_t z_msg;
  zmq::pollitem_t z_items[] {
    {         0, fd, ZMQ_POLLIN, 0 },
    { broadcast,  0, ZMQ_POLLIN, 0 }
  };
  int more;
  while(true)
  {
    zmq::poll(&z_items[0], 2, -1);

    if ( z_items[0].revents & ZMQ_POLLIN )
    {
      int length, i = 0;
      char buffer[SB_IN_BUF_LEN];
      
      length = read( fd, buffer, SB_IN_BUF_LEN );
      if ( length < 0 ) perror("inotify poll");

      while( i < length )
      {
        struct inotify_event* event;
        event = (struct inotify_event*) &buffer[0];
        sstream << event->wd     << " "
                << event->mask   << " "
                << event->cookie << " "
                << event->len    << "\n"
                << event->name;
        i += SB_IN_EVENT_SIZE + event->len;
        if ( i < length ) sstream << "\n";
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