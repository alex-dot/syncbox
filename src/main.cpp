/*
 * Blablub
 *
 * main.cpp
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zmq.hpp>

#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <boost/thread.hpp>

#include <signal.h>

#include "constants.hpp"
#include "boxoffice.hpp"
#include "publisher.hpp"

void *publisher(void *arg)
{
  Publisher* pub;
  pub = Publisher::initialize(static_cast<zmq::context_t*>(arg));

  pub->run();
  pub->sendExitSignal();

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
  snprintf((char*) z_msg_close.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
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

    // we eagerly initialize the Boxoffice- and Publisher-singleton here, 
    // for thread-safety
    Boxoffice* bo;
    bo = Boxoffice::getInstance();
    Publisher* pub;
    pub = Publisher::getInstance();

    // bind to boxoffice endpoint
    zmq::socket_t z_boxoffice(z_context, ZMQ_PAIR);
    z_boxoffice.bind("inproc://sb_bo_main_pair");

    std::cout << "main: opening boxoffice thread" << std::endl;
    boost::thread bo_thread(boxoffice_thread, &z_context);

    // open publisher thread
    std::cout << "main: opening publisher thread" << std::endl;
    boost::thread pub_thread(publisher, &z_context);

    // open subscriber thread
    std::cout << "main: opening subscriber thread" << std::endl;
    boost::thread sub_thread(subscriber, &z_context);

    // wait for signal from boxoffice
    zmq::message_t z_msg_close;
    int msg_type, msg_signal;
    std::cout << "main: waiting for boxoffice to send exit signal" << std::endl;
    z_boxoffice.recv(&z_msg_close);
    std::istringstream iss_pub(static_cast<char*>(z_msg_close.data()));
    iss_pub >> msg_type >> msg_signal;
    iss_pub.clear();

    std::cout << "main: received message: " << msg_type << " " << msg_signal << std::endl;

    bo_thread.join();
    pub_thread.join();
    sub_thread.join();

    z_boxoffice.close();

    // TODO: prog hangs when just returning or trying to destroy context
//    zmq_ctx_destroy(z_context);

    exit(0);
//    return 0;

/*
    return 0;
  } // pid_sub > 0 is the parent process
  else if (pid_sub > 0)
  {
    exit(EXIT_SUCCESS);
  }*/
}