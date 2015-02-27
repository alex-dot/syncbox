/*
 * Boxoffice
 */

#include "boxoffice.hpp"

#include <zmq.hpp>
#include <sstream>
#include <iostream>

Boxoffice* Boxoffice::initialize(zmq::context_t* z_ctx)
{
  Boxoffice* bo = getInstance();

  bo->connectToPubsAndSubs(z_ctx);

  return bo;
}

int Boxoffice::connectToPubsAndSubs(zmq::context_t* z_ctx)
{
  zmq::context_t *z_context = static_cast<zmq::context_t*>(z_ctx);
  
  // open PAIR sender to main thread
  zmq::socket_t z_bo_main(*z_context, ZMQ_PAIR);
  z_bo_main.connect("inproc://sb_bo_main_pair");

  zmq::socket_t z_subscribers(*z_context, ZMQ_ROUTER);
  zmq::socket_t z_publishers(*z_context, ZMQ_DEALER);

  z_subscribers.bind("inproc://sb_boxoffice_req");
  z_publishers.bind("inproc://sb_boxoffice_rep");

  std::cout << "bo: starting to listen to subscriber..." << std::endl;

  zmq::pollitem_t z_items[] = {
    { z_subscribers, 0, ZMQ_POLLIN, 0 },
    { z_publishers , 0, ZMQ_POLLIN, 0 }
  };

  while(true) {
    bool exit_signal = false;
    int more;

    zmq::poll(&z_items[0], 2, -1);

    if (z_items[0].revents & ZMQ_POLLIN) {
      while(true) {
        int msg_type, msg_signal;
        zmq::message_t z_msg;

        std::cout << "bo: waiting for subscriber to send exit signal" << std::endl;
        z_subscribers.recv(&z_msg);
        size_t more_size = sizeof(more);
        z_subscribers.getsockopt(ZMQ_RCVMORE, &more, &more_size);

        std::istringstream iss_sub(static_cast<char*>(z_msg.data()));
        iss_sub >> msg_type >> msg_signal;
        std::cout << "bo: sub sent: " << msg_type << " " << msg_signal << std::endl;
        if ( msg_type == 0 && msg_signal == 0 )
        {
          snprintf((char*) z_msg.data(), 4, "0 0");
          z_subscribers.send(z_msg, more? ZMQ_SNDMORE: 0);
          z_publishers.send(z_msg, more? ZMQ_SNDMORE: 0);
          break;
        } else {
          snprintf((char*) z_msg.data(), 4, "0 1");
          z_subscribers.send(z_msg);
        }
      }
    }
    if (z_items[1].revents & ZMQ_POLLIN) {
      while(true) {
        int msg_type, msg_signal;
        zmq::message_t z_msg;

        std::cout << "bo: waiting for publishers to send exit signal" << std::endl;
        z_publishers.recv(&z_msg);
        size_t more_size = sizeof(more);
        z_publishers.getsockopt(ZMQ_RCVMORE, &more, &more_size);

        std::istringstream iss_pub(static_cast<char*>(z_msg.data()));
        iss_pub >> msg_type >> msg_signal;
        std::cout << "bo: pub sent: " << msg_type << " " << msg_signal << std::endl;
        if ( msg_type == 0 && msg_signal == 0 )
        {
          exit_signal = true;
          break;
        }
      }
    }

    if (exit_signal) break;
  }

  std::cout << "bo: exit signal received, sending signal..." << std::endl;
  zmq::message_t z_msg_close(3);
  snprintf((char*) z_msg_close.data(), 4, "%d %d", 0, 0);
  z_bo_main.send(z_msg_close);
  std::cout << "bo: exit signal sent, exiting." << std::endl;

//  z_subscribers.close();
//  z_publishers.close();

  return 0;
}