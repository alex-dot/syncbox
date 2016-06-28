/*
 * generic constants and globals for application
 */

#ifndef SB_CONSTANTS_HPP
#define SB_CONSTANTS_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include <signal.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <ctime>

enum SB_SIGTYPE {
  SB_SIGTYPE_LIFE,
  SB_SIGTYPE_PUB,
  SB_SIGTYPE_SUB,
  SB_SIGTYPE_INOTIFY,
  SB_SIGTYPE_FSM
};
enum SB_SIGLIFE {
  SB_SIGLIFE_ALIVE,
  SB_SIGLIFE_EXIT,
  SB_SIGLIFE_INTERRUPT,
  SB_SIGLIFE_ERROR=-1
};
enum SB_SIGPUB {
  SB_SIGPUB_GET_CHANNELS
};
enum SB_SIGSUB {
  SB_SIGSUB_GET_CHANNELS
};
enum SB_SUB_TYPE {
  SB_SUBTYPE_TCP_BIDIR,
  SB_SUBTYPE_TCP_UNIDIR
};
enum SB_BACKUP_TYPE {
  SB_BACKUP_NONE,
  SB_BACKUP_LOCAL,
  SB_BACKUP_REMOTE,
  SB_BACKUP_BOTH
};

#define SB_MSG_DEBUG true

//static size_t  
#define SB_IN_EVENT_SIZE (sizeof(struct inotify_event))
//static size_t  
#define SB_IN_BUF_LEN    (1024 * (SB_IN_EVENT_SIZE + 16))
#define SB_IN_EVENT_MASK  IN_ATTRIB|IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_MODIFY|IN_MOVE|IN_MOVE_SELF


#define SB_CONFIG_FILE "~/.syncbox"

// file message struct
struct file_msg_string
{
  std::string tag;
  std::time_t time;
  std::string text;
};

// wrapper for polling on one socket while simultaneously polling the broadcast
void s_recv(zmqpp::socket &socket, zmqpp::socket &broadcast, std::stringstream &sstream);
void s_recv(zmqpp::socket &socket, zmqpp::socket &broadcast, zmqpp::socket &heartbeat, std::stringstream &sstream);
// wrapper for polling on one socket while simultaneously polling the broadcast, but non-blocking
int s_recv_noblock(zmqpp::socket &socket, zmqpp::socket &broadcast, std::stringstream &sstream, int timeout);
// wrapper for polling on inotify event while simultaneously polling the broadcast
void s_recv_in(zmqpp::socket &broadcast, zmqpp::socket &socket, int fd, std::stringstream &sstream);

#endif