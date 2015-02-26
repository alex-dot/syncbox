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
    zmq::context_t zcontext(1);
std::cout << "opened context" << std::endl;
    zmq::socket_t zsubscriber(zcontext, ZMQ_SUB);
std::cout << "bound socket" << std::endl;
    zsubscriber.connect("ipc://syncbox.ipc");
    const char *sub_filter = "";
    zsubscriber.setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);
std::cout << "subscribed" << std::endl;

    for (int i = 0; i < 10; ++i)
    {
      std::cout << "message " << i << std::endl;
      zmq::message_t zmessage;
      zsubscriber.recv(&zmessage);

      std::string msg;
      std::istringstream iss(static_cast<char*>(zmessage.data()));
      iss >> msg;
      std::cout << iss.str() << std::endl;
    }
/*
    return 0;
  } // pid_sub > 0 is the parent process
  else if (pid_sub > 0)
  {
    exit(EXIT_SUCCESS);
  }*/
}