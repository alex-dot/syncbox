


#include "constants.hpp"

// wrapper for polling on one socket while simultaneously polling the broadcast
void s_recv(zmq::socket_t &socket, zmq::socket_t &broadcast, std::stringstream &sstream)
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
void s_recv_in(zmq::socket_t &broadcast, int fd, std::stringstream &sstream)
{
  zmq::message_t z_msg;
  zmq::pollitem_t z_items[] {
    {         0, fd, ZMQ_POLLIN, 0 },
    { broadcast,  0, ZMQ_POLLIN, 0 }
  };
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