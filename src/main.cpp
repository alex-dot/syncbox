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

static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
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
//void *boxoffice_thread(void *arg, void *ctx_)
{
  Boxoffice* bo;
  bo = Boxoffice::initialize(static_cast<zmq::context_t*>(arg));

/*
  zmq::context_t* z_context(static_cast<zmq::context_t*>(ctx_));

    int* fd = static_cast<int*>(arg); 

    zmq::socket_t test(*z_context, ZMQ_SUB);
    test.connect("inproc://sb_broadcast");
  const char *sub_filter = "";
  test.setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);

std::cout << "receiving msg" << std::endl;
  std::stringstream* sstream = new std::stringstream();
  s_recv_in(test, *fd, *sstream);
  std::cout << sstream->str() << std::endl;
  delete sstream;

  test.close();
*/
/*
    pollfd pollfd = {*fd, POLLIN, 0};
    poll(&pollfd, 1, -1);
    std::cout << "blub" << std::endl;
    length = read( *fd, buffer, SB_IN_BUF_LEN );
    if ( length < 0 ) perror("inotify poll");
    std::cout << "blub" << std::endl;

    struct inotify_event* event;
    event = (struct inotify_event*) &buffer[0];
    std::cout << event->mask << std::endl;
*/

  return (NULL);
}

int main(int argc, char* argv[])
{
  // FORKING
  /*
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
*/
    // opening zeromq context
    zmq::context_t z_context(1);

    // we eagerly initialize the Boxoffice- and Publisher-singleton here, 
    // for thread-safety
    Boxoffice::getInstance();
    Publisher::getInstance();

    // catch signals
    s_catch_signals();


/*
    // bind process broadcast pub
    zmq::socket_t test(z_context, ZMQ_PUB);
    test.bind("inproc://sb_broadcast");

    int length, i = 0;
    int fd; 
    int wd;
    char buffer[SB_IN_BUF_LEN];

    fd = inotify_init();
    if ( fd < 0 ) perror("inotify_init");
    wd = inotify_add_watch( fd, "/home/alex/bin/syncbox/test/testdir2", SB_IN_EVENT_MASK );

    if (SB_MSG_DEBUG) printf("main: opening boxoffice thread\n");
    boost::thread bo_thread(boxoffice_thread, &fd, &z_context);
*/

/*
    std::cout << IN_ACCESS << std::endl;
    std::cout << IN_ATTRIB << std::endl;
    std::cout << IN_CLOSE_WRITE << std::endl;
    std::cout << IN_CLOSE_NOWRITE << std::endl;
    std::cout << IN_CREATE << std::endl;
    std::cout << IN_DELETE << std::endl;
    std::cout << IN_DELETE_SELF << std::endl;
    std::cout << IN_MODIFY << std::endl;
    std::cout << IN_MOVE_SELF << std::endl;
    std::cout << IN_MOVED_FROM << std::endl;
    std::cout << IN_MOVED_TO << std::endl;
    std::cout << IN_OPEN << std::endl;
    std::cout << IN_MOVE << std::endl;
    std::cout << IN_CLOSE << std::endl;
    std::cout << IN_DONT_FOLLOW << std::endl;
    std::cout << IN_EXCL_UNLINK << std::endl;
    std::cout << IN_MASK_ADD << std::endl;
    std::cout << IN_ONESHOT << std::endl;
    std::cout << IN_IGNORED << std::endl;
    std::cout << IN_ISDIR << std::endl;
    std::cout << IN_Q_OVERFLOW << std::endl;
    std::cout << IN_UNMOUNT << std::endl;
*/

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
        std::cout << "main: received interrupt, broadcasting signal..." << std::endl;
        snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_INTERRUPT);
        z_broadcast.send(z_msg);
        break;
      }

      int msg_type, msg_signal;
      sstream >> msg_type >> msg_signal;
      if (SB_MSG_DEBUG) printf("main: received message: %d %d\n", msg_type, msg_signal);
      if ( msg_type != SB_SIGTYPE_LIFE || msg_signal != SB_SIGLIFE_EXIT ) return 1;
      else break;
    }

    bo_thread.join();

    z_boxoffice.close();
    z_broadcast.close();


/*
    sleep(4);
    std::cout << "closing thread" << std::endl;


      zmq::message_t z_msg;
        snprintf((char*) z_msg.data(), 7, "blabla");
        test.send(z_msg);


//    bo_thread.interrupt();
    std::cout << "closed" << std::endl;
    sleep(1);

    inotify_rm_watch(fd, wd);
    close(fd);

test.close();
*/
    z_context.close();


    return 0;

/*
    return 0;
  } // pid > 0 is the parent process
  else if (pid > 0)
  {
    exit(EXIT_SUCCESS);
  }*/
}