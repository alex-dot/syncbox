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
#include <sys/inotify.h>
#include <poll.h>

#include "constants.hpp"
#include "boxoffice.hpp"
#include "publisher.hpp"
#include "config.hpp"

static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = signal_value;
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


void *boxoffice_thread(void *arg)
{
  Boxoffice::initialize(static_cast<zmq::context_t*>(arg));
  return (NULL);
}


int main_application(int argc, char* argv[])
{
  // opening zeromq context
  zmq::context_t z_context(1);

  // we eagerly initialize all singletons here
  Boxoffice::getInstance();
  int return_val = Config::initialize(argc, argv);
  if (return_val != 0) {
    return return_val;
  }

  // catch signals
  s_catch_signals();

  // bind process broadcast pub
  zmq::socket_t z_broadcast(z_context, ZMQ_PUB);
  z_broadcast.bind("inproc://sb_broadcast");
  // bind to boxoffice endpoint
  zmq::socket_t z_boxoffice(z_context, ZMQ_PAIR);
  z_boxoffice.bind("inproc://sb_bo_main_pair");

  // opening boxoffice thread
  if (SB_MSG_DEBUG) printf("main: opening boxoffice thread\n");
  boost::thread bo_thread(boxoffice_thread, &z_context);

  // wait for signal from boxoffice
  if (SB_MSG_DEBUG) printf("main: waiting for boxoffice to send exit signal\n");
  while(true)
  {
    zmq::message_t z_msg;
    std::stringstream sstream;

    try {
      z_boxoffice.recv(&z_msg);
      sstream << static_cast<char*>(z_msg.data());
    } catch(const zmq::error_t &e) {
      std::cout << "E: " << e.what() << std::endl;
    }

    if (s_interrupted)
    {
      std::cout << "main: received interrupt, broadcasting signal to boxoffice..." << std::endl;
      snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_INTERRUPT);
      z_boxoffice.send(z_msg);
      std::cout << "main: waiting for boxoffice to send exit..." << std::endl;
      z_boxoffice.recv(&z_msg);
      sstream << static_cast<char*>(z_msg.data());
      int msg_type, msg_signal;
      sstream >> msg_type >> msg_signal;
      std::cout << "main: boxoffice exited, broadcasting signal..." << std::endl;
      snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_INTERRUPT);
      z_broadcast.send(z_msg);

      break;
    }

    int msg_type, msg_signal;
    sstream >> msg_type >> msg_signal;
    if (SB_MSG_DEBUG) printf("main: received message: %d %d\n", msg_type, msg_signal);
    if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_EXIT ) return 1;
   // else break;
  }

  bo_thread.join();

  z_boxoffice.close();
  z_broadcast.close();

//  z_context.close();

  return 0;
}

int main(int argc, char* argv[])
{
  // FORKING
  if (SB_MSG_DEBUG)
  {
    return main_application(argc, argv);
  } else {
    pid_t pid, sid;
  
    // fork off the SUB process
    pid = fork();
    // if pid < 0 the forking failed
    if (pid < 0)
    {
      exit(EXIT_FAILURE);
    } // pid==0 is the SUB process itself
    else if (pid == 0)
    {
      umask(0);
      sid = setsid();
      if (sid < 0)
      {
        exit(EXIT_FAILURE);
      }
  
      return main_application(argc, argv);
  
    } // pid > 0 is the parent process
    else if (pid > 0)
    {
      exit(EXIT_SUCCESS);
    }
  }

}