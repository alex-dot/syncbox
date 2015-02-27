/*
 * Blablub
 *
 * main.cpp
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zmq.hpp>

#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <pthread.h>

#include "boxoffice.hpp"

void *publisher(void *arg)
{
  std::cout << "pub: starting pubsub sockets and sending..." << std::endl;

  zmq::context_t *z_context = static_cast<zmq::context_t*>(arg);

  zmq::socket_t z_boxoffice(*z_context, ZMQ_REP);
  zmq::socket_t z_publisher(*z_context, ZMQ_PUB);

  z_boxoffice.connect("inproc://sb_boxoffice_rep");
  z_publisher.bind("ipc://syncbox.ipc");

  for (int i = 0; i < 5; ++i)
  {
    std::string message = "this is message: " + std::to_string(i);
    std::cout << "pub: " << message << std::endl;
    zmq::message_t z_msg(message.length()+1);
    snprintf((char*) z_msg.data(), message.length()+1, "%s", message.c_str());
    z_publisher.send(z_msg);
    sleep(1);
  }

  std::cout << "pub: all messages sent, waiting for close signal..." << std::endl;
  while(1)
  {
    int msg_type, msg_signal;
    zmq::message_t z_msg_close;
    z_boxoffice.recv(&z_msg_close);
    std::istringstream iss(static_cast<char*>(z_msg_close.data()));
    iss >> msg_type >> msg_signal;
    std::cout << "pub: boxoffice sent: " << msg_type << " " << msg_signal << std::endl;
    if ( msg_type == 0 && msg_signal == 0 )
    {
      std::cout << "pub: received exit code, sending signal..." << std::endl;
      snprintf((char*) z_msg_close.data(), 4, "%d %d", 0, 0);
      z_boxoffice.send(z_msg_close);
      std::cout << "pub: exit signal sent, exiting." << std::endl; 
      break;
    } else {
      snprintf((char*) z_msg_close.data(), 4, "%d %d", 0, 1);
      z_boxoffice.send(z_msg_close);
    }
  }

  return (NULL);
}

void *subscriber(void *arg)
{
  std::cout << "sub: receiving from publisher..." << std::endl;

  zmq::context_t *z_context = static_cast<zmq::context_t*>(arg);

  zmq::socket_t z_boxoffice(*z_context, ZMQ_REQ);
  zmq::socket_t z_subscriber(*z_context, ZMQ_SUB);

  z_boxoffice.connect("inproc://sb_boxoffice_req");
  z_subscriber.connect("ipc://syncbox.ipc");
  const char *sub_filter = "";
  z_subscriber.setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);

  for (int i = 0; i < 3; ++i)
  {
    zmq::message_t z_message;
    z_subscriber.recv(&z_message);

    std::istringstream iss(static_cast<char*>(z_message.data()));
    std::cout << "sub:" << iss.str() << std::endl;
  }

  std::cout << "sub: all messages received, sending signal..." << std::endl;
  zmq::message_t z_msg_close(3);
  snprintf((char*) z_msg_close.data(), 4, "%d %d", 0, 0);
  z_boxoffice.send(z_msg_close);
  std::cout << "sub: signal sent, exiting..." << std::endl;

  return (NULL);
}

void *boxoffice_thread(void *arg)
{
  Boxoffice* bo;
  bo = Boxoffice::initialize(static_cast<zmq::context_t*>(arg));
  return (NULL);
}

int main(int argc, char* argv[])
{
  // FORKING
  /*
  pid_t pid_sub, sid_sub;

  // fork off the SUB process
  pid_sub = fork();
  // if pid_sub < 0 the forking failed
  if (pid_sub < 0)
  {
    exit(EXIT_FAILURE);
  } // pid_sub==0 is the SUB process itself
  else if (pid_sub == 0)
  {
    umask(0);
    sid_sub = setsid();
    if (sid_sub < 0)
    {
      exit(EXIT_FAILURE);
    }
*/

    zmq::context_t z_context(1);

    Boxoffice* bo;
    // we eagerly initialize the Boxoffice-singleton here, for thread-safety
    bo = Boxoffice::getInstance();
    std::cout << "main: opening boxoffice thread" << std::endl;
    pthread_t bo_thread;
    pthread_create(&bo_thread, NULL, boxoffice_thread, &z_context);
//    bo = Boxoffice::initialize(&z_context);

    // bind to boxoffice endpoint
    zmq::socket_t z_boxoffice(z_context, ZMQ_PAIR);
    z_boxoffice.bind("inproc://sb_bo_main_pair");

    // bind to publisher endpoint
//    zmq::socket_t z_pub_receiver(z_context, ZMQ_PAIR);
//    z_pub_receiver.bind("inproc://sb_pub_main_pair");
    // bind to subscriber endpoint
//    zmq::socket_t z_sub_receiver(z_context, ZMQ_PAIR);
//    z_sub_receiver.bind("inproc://sb_sub_main_pair");

    // open publisher thread
    std::cout << "main: opening publisher thread" << std::endl;
    pthread_t pub_thread;
    pthread_create(&pub_thread, NULL, publisher, &z_context);

    // open subscriber thread
    std::cout << "main: opening subscriber thread" << std::endl;
    pthread_t sub_thread;
    pthread_create(&sub_thread, NULL, subscriber, &z_context);

    // wait for signals from threads
    zmq::message_t z_msg_close;
    int msg_type, msg_signal;
    msg_type = 99;
    msg_signal = 111;
    // wait for signal from boxoffice
    std::cout << "main: waiting for boxoffice to send exit signal" << std::endl;
    z_boxoffice.recv(&z_msg_close);
    std::istringstream iss_pub(static_cast<char*>(z_msg_close.data()));
    iss_pub >> msg_type >> msg_signal;
    // wait for signal from publisher
//    std::cout << "main: waiting for subscriber to send exit signal" << std::endl;
//    z_pub_receiver.recv(&z_msg_close);
//    std::istringstream iss_pub(static_cast<char*>(z_msg_close.data()));
//    iss_pub >> msg_type >> msg_signal;
    // wait for signal from subscriber
//    std::cout << "main: waiting for subscriber to send exit signal" << std::endl;
//    z_sub_receiver.recv(&z_msg_close);
//    std::istringstream iss_sub(static_cast<char*>(z_msg_close.data()));
//    iss_sub >> msg_type >> msg_signal;

    std::cout << "main: received message: " << msg_type << " " << msg_signal << std::endl;

//    z_boxoffice.close();
//    z_context.close();

    return 0;

/*
    return 0;
  } // pid_sub > 0 is the parent process
  else if (pid_sub > 0)
  {
    exit(EXIT_SUCCESS);
  }*/
}