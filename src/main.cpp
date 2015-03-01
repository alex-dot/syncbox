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
    // opening zeromq context
    zmq::context_t z_context(1);

    // we eagerly initialize the Boxoffice- and Publisher-singleton here, 
    // for thread-safety
    Boxoffice::getInstance();
    Publisher::getInstance();

    // catch signals
    s_catch_signals();

    // bind to boxoffice endpoint
    zmq::socket_t z_boxoffice(z_context, ZMQ_PAIR);
    z_boxoffice.bind("inproc://sb_bo_main_pair");

    // opening boxoffice thread
    std::cout << "main: opening boxoffice thread" << std::endl;
    boost::thread bo_thread(boxoffice_thread, &z_context);

    // wait for signal from boxoffice
    std::cout << "main: waiting for boxoffice to send exit signal" << std::endl;
    while(true)
    {
      zmq::message_t z_msg;
      std::stringstream sstream;
      try {
        z_boxoffice.recv(&z_msg);
        sstream << static_cast<char*>(z_msg.data());
      } catch(const zmq::error_t &e) {
        std::cout << "error happened" << std::endl;
      }
      if (s_interrupted)
      {
        std::cout << "interrupted!" << std::endl;
        break;
      }

      int msg_type, msg_signal;
      sstream >> msg_type >> msg_signal;
      std::cout << "main: received message: " << msg_type << " " << msg_signal << std::endl;
      if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_EXIT ) return 1;
      else break;
    }

    bo_thread.join();

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