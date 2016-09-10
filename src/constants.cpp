// TODO removed while loops, for it seems as though it waits at poll anyway 
// and returns once something happened


#include "constants.hpp"

// wrapper for polling on one socket while simultaneously polling the broadcast
void s_recv(zmqpp::socket &socket, zmqpp::socket &broadcast, std::stringstream &sstream)
{
  zmqpp::message z_msg;
  zmq_pollitem_t z_items[] = {
    { static_cast<void *>(socket),    0, ZMQ_POLLIN, 0 },
    { static_cast<void *>(broadcast), 0, ZMQ_POLLIN, 0 }
  };

  zmqpp::poller poller;
  poller.add(z_items[0]);
  poller.add(z_items[1]);

  poller.poll();

  if ( poller.events(z_items[0]) & ZMQ_POLLIN )
  {
    socket.receive(z_msg);
    int parts = z_msg.parts();
    for (int i = 0; i < parts; ++i)
    {
      sstream << z_msg.get(i);
    }
  }
  else if ( poller.events(z_items[1]) & ZMQ_POLLIN )
  {
    broadcast.receive(z_msg);
    sstream << z_msg.get(0);
  }
}
void s_recv(zmqpp::socket &socket,
            zmqpp::socket &broadcast,
            zmqpp::socket &heartbeat,
            std::stringstream &sstream)
{
  zmqpp::message z_msg;
  zmq_pollitem_t z_items[] = {
    { static_cast<void *>(socket),    0, ZMQ_POLLIN, 0 },
    { static_cast<void *>(broadcast), 0, ZMQ_POLLIN, 0 },
    { static_cast<void *>(heartbeat), 0, ZMQ_POLLIN, 0 }
  };

  zmqpp::poller poller;
  poller.add(z_items[0]);
  poller.add(z_items[1]);
  poller.add(z_items[2]);

  poller.poll();

  if ( poller.events(z_items[0]) & ZMQ_POLLIN )
  {
    socket.receive(z_msg);
    int parts = z_msg.parts();
    for (int i = 0; i < parts; ++i)
    {
      sstream << z_msg.get(i);
    }
  }
  if ( poller.events(z_items[1]) & ZMQ_POLLIN )
  {
    broadcast.receive(z_msg);
    sstream << z_msg.get(0);
  }
  if ( poller.events(z_items[2]) & ZMQ_POLLIN )
  {
    heartbeat.receive(z_msg);
    sstream << z_msg.get(0);
  }
}
void s_recv(zmqpp::socket &socket,
            zmqpp::socket &broadcast,
            zmqpp::socket &heartbeat,
            zmqpp::socket &dispatch,
            std::stringstream &sstream)
{
  zmqpp::message z_msg;
  zmq_pollitem_t z_items[] = {
    { static_cast<void *>(socket),    0, ZMQ_POLLIN, 0 },
    { static_cast<void *>(broadcast), 0, ZMQ_POLLIN, 0 },
    { static_cast<void *>(heartbeat), 0, ZMQ_POLLIN, 0 },
    { static_cast<void *>(dispatch), 0, ZMQ_POLLIN, 0 }
  };

  zmqpp::poller poller;
  poller.add(z_items[0]);
  poller.add(z_items[1]);
  poller.add(z_items[2]);
  poller.add(z_items[3]);

  poller.poll();

  if ( poller.events(z_items[0]) & ZMQ_POLLIN )
  {
    socket.receive(z_msg);
    int parts = z_msg.parts();
    for (int i = 0; i < parts; ++i)
    {
      sstream << z_msg.get(i);
    }
  }
  if ( poller.events(z_items[1]) & ZMQ_POLLIN )
  {
    broadcast.receive(z_msg);
    sstream << z_msg.get(0);
  }
  if ( poller.events(z_items[2]) & ZMQ_POLLIN )
  {
    heartbeat.receive(z_msg);
    sstream << z_msg.get(0);
  }
  if ( poller.events(z_items[3]) & ZMQ_POLLIN )
  {
    dispatch.receive(z_msg);
    sstream << z_msg.get(0);
  }
}

// wrapper for polling on three sockets, but non-blocking
int s_recv_noblock(zmqpp::socket &socket, zmqpp::socket &socket2, zmqpp::socket &broadcast, std::stringstream &sstream, int timeout)
{
  int return_val;
  zmqpp::message z_msg;
  zmq_pollitem_t z_items[] = {
    { static_cast<void *>(socket),    0, ZMQ_POLLIN, 0 },
    { static_cast<void *>(socket2),   0, ZMQ_POLLIN, 0 },
    { static_cast<void *>(broadcast), 0, ZMQ_POLLIN, 0 }
  };

  zmqpp::poller poller;
  poller.add(z_items[0]);
  poller.add(z_items[1]);
  poller.add(z_items[2]);

  if ( poller.poll(timeout) ) {
    if ( poller.events(z_items[0]) & ZMQ_POLLIN )
    {
      socket.receive(z_msg);
      int parts = z_msg.parts();
      for (int i = 0; i < parts; ++i)
      {
        sstream << z_msg.get(i);
      }
    }
    if ( poller.events(z_items[1]) & ZMQ_POLLIN )
    {
      socket2.receive(z_msg);
      int parts = z_msg.parts();
      for (int i = 0; i < parts; ++i)
      {
        sstream << z_msg.get(0);
      }
    }
    if ( poller.events(z_items[2]) & ZMQ_POLLIN )
    {
      broadcast.receive(z_msg);
      sstream << z_msg.get(0);
    }
    return_val = 1;
  } else {
    return_val = 0;
  }
  return return_val;
}

// wrapper for polling on inotify event while simultaneously polling the broadcast
void s_recv_in(zmqpp::socket &broadcast, zmqpp::socket &socket, int fd, std::stringstream &sstream)
{
  zmqpp::message z_msg;
  zmq_pollitem_t z_items[] {
    {                        nullptr, fd, ZMQ_POLLIN, 0 },
    { static_cast<void *>(broadcast),  0, ZMQ_POLLIN, 0 },
    { static_cast<void *>(socket),     0, ZMQ_POLLIN, 0 }
  };

  zmqpp::poller poller;
  poller.add(z_items[0]);
  poller.add(z_items[1]);
  poller.add(z_items[2]);

  poller.poll();

  if ( poller.events(z_items[0]) & ZMQ_POLLIN )
  {
    sstream << F_SIGTYPE_INOTIFY << " ";

    int length, i = 0;
    char buffer[F_IN_BUF_LEN];

    length = read( fd, buffer, F_IN_BUF_LEN );
    if ( length < 0 ) perror("inotify poll");

    while( i < length )
    {
      struct inotify_event* event;
      event = (struct inotify_event*) &buffer[i];
      sstream << event->mask << " ";
      sstream << event->wd   << " ";
      sstream << event->name;
      i += F_IN_EVENT_SIZE + event->len;
      if ( i < length ) sstream << "\n";
    }
  }
  if ( poller.events(z_items[1]) & ZMQ_POLLIN )
  {
      broadcast.receive(z_msg);
      sstream << z_msg.get(0);
  }
  if ( poller.events(z_items[2]) & ZMQ_POLLIN )
  {
      broadcast.receive(z_msg);
      sstream << z_msg.get(0);
  }
}