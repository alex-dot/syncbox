/*
 * Blablub
 *
 * main.cpp
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zmqpp/zmqpp.hpp>

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
  Boxoffice::initialize(static_cast<zmqpp::context*>(arg));
  return (NULL);
}


int main_application(int argc, char* argv[])
{
  // opening zeromq context
  zmqpp::context z_context;

  // we eagerly initialize all singletons here
  Boxoffice::getInstance();
  int return_val = Config::initialize(argc, argv);
  if (return_val != 0) {
    return return_val;
  }

  // catch signals
  s_catch_signals();

  // bind process broadcast pub
  zmqpp::socket z_broadcast(z_context, zmqpp::socket_type::pub);
  z_broadcast.bind("inproc://sb_broadcast");
  // bind to boxoffice endpoint
  zmqpp::socket z_boxoffice(z_context, zmqpp::socket_type::pair);
  z_boxoffice.bind("inproc://sb_bo_main_pair");

  // opening boxoffice thread
  if (SB_MSG_DEBUG) printf("main: opening boxoffice thread\n");
  boost::thread bo_thread(boxoffice_thread, &z_context);

  // wait for signal from boxoffice
  if (SB_MSG_DEBUG) printf("main: waiting for boxoffice to send exit signal\n");
  while(s_interrupted == 0)
  {
    usleep(100);
  }

  std::cout << "main: received interrupt, broadcasting signal to boxoffice..." << std::endl;
  std::stringstream* message = new std::stringstream();
  *message << SB_SIGTYPE_LIFE << " " << SB_SIGLIFE_INTERRUPT;
  zmqpp::message* z_msg = new zmqpp::message();
  *z_msg << message->str();
  z_boxoffice.send(*z_msg);
  delete z_msg;

  std::cout << "main: boxoffice exited, broadcasting signal..." << std::endl;
  message = new std::stringstream();
  *message << SB_SIGTYPE_LIFE << " " << SB_SIGLIFE_INTERRUPT;
  z_msg = new zmqpp::message();
  *z_msg << message->str();
  z_broadcast.send(*z_msg);

  std::cout << "main: waiting for boxoffice to send exit..." << std::endl;
  z_msg = new zmqpp::message();
  z_boxoffice.receive(*z_msg);
  std::stringstream sstream;
  sstream << z_msg->get(0);
  int msg_type, msg_signal;
  sstream >> msg_type >> msg_signal;
  delete z_msg;

  sstream >> msg_type >> msg_signal;
  if ( msg_type != SB_SIGTYPE_LIFE && msg_signal != SB_SIGLIFE_EXIT ) return 1;
  if (SB_MSG_DEBUG) printf("main: boxoffice sent exit signal, cleaning up and exiting...");

  bo_thread.join();

  z_boxoffice.close();
  z_broadcast.close();

  z_context.terminate();

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