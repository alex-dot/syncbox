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

void *publisher(void *arg)
{
  zmq::context_t *z_context = static_cast<zmq::context_t*>(arg);
  // open PAIR sender to main thread
  zmq::socket_t z_main_sender(*z_context, ZMQ_PAIR);
  z_main_sender.connect("inproc://sb_pub_main_pair");
  // open PAIR sender to subscriber thread
  zmq::socket_t z_pubsub_sender(*z_context, ZMQ_PAIR);
  z_pubsub_sender.connect("inproc://sb_pub_to_sub");
  // bind to PAIR receiver from subscriber thread
  zmq::socket_t z_pubsub_receiver(*z_context, ZMQ_PAIR);
  z_pubsub_receiver.connect("inproc://sb_sub_to_pub");

  std::cout << "starting pubsub sockets and sending..." << std::endl;

  zmq::socket_t z_publisher(*z_context, ZMQ_PUB);
  z_publisher.bind("ipc://syncbox.ipc");

  for (int i = 0; i < 30; ++i)
  {
    std::string message = "this is message: " + std::to_string(i);
    std::cout << message << std::endl;
    zmq::message_t z_msg(message.length()+1);
    snprintf((char*) z_msg.data(), message.length()+1, "%s", message.c_str());
    z_publisher.send(z_msg);
    sleep(1);
  }

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
    // bind to publisher endpoint
    zmq::socket_t z_receiver(z_context, ZMQ_PAIR);
    z_receiver.bind("inproc://sb_pub_main_pair");
    // open publisher thread
    pthread_t pub_thread;
    pthread_create(&pub_thread, NULL, publisher, &z_context);

    // wait for signal from publisher
    zmq::message_t pub_msg;
    z_receiver.recv(&pub_msg);

    return 0;

/*
    return 0;
  } // pid_sub > 0 is the parent process
  else if (pid_sub > 0)
  {
    exit(EXIT_SUCCESS);
  }*/
}