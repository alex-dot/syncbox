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

int main(int argc, char* argv[])
{
  // FORKING
/*  pid_t pid_pub, sid_pub;

  // fork off the PUB process
  pid_pub = fork();
  // if pid_pub < 0 the forking failed
  if (pid_pub < 0)
  {
    exit(EXIT_FAILURE);
  } // pid_pub==0 is the PUB process itself
  else if (pid_pub == 0)
  {
    umask(0);
    sid_pub = setsid();
    if (sid_pub < 0)
    {
      exit(EXIT_FAILURE);
    }
*/
    zmq::context_t zcontext(1);
std::cout << "opened context" << std::endl;
    zmq::socket_t zpublisher(zcontext, ZMQ_PUB);
std::cout << "bound socket" << std::endl;
    zpublisher.bind("ipc://syncbox.ipc");
std::cout << "subscribed" << std::endl;

    for (int i = 0; i < 30; ++i)
    {
      std::string message = "this is message: " + std::to_string(i);
      std::cout << message << std::endl;
      zmq::message_t zmsg(message.length()+1);
      snprintf((char*) zmsg.data(), message.length()+1, "%s", message.c_str());
      zpublisher.send(zmsg);
      sleep(2);
    }
/*
    return 0;
  } // pid_pub > 0 is the parent process
  else if (pid_pub > 0)
  {
    exit(EXIT_SUCCESS);
  }*/
}