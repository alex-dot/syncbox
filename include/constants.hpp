/*
 * generic constants and globals for application
 */

#ifndef SB_CONSTANTS_HPP
#define SB_CONSTANTS_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include <zmqpp/curve.hpp>
#include <signal.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <ctime>

#include <hash.hpp>

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

#define SB_GENERIC_HASH_LEN 256

#define SB_MINIMUM_HB_WIDTH 256

#define SB_CONFIG_FILE "~/.syncbox"
#define SB_KEYSTORE_FILE "~/.ssh/syncbox_keystore"
#define SB_PRIVATEKEY_FILE "~/.ssh/syncbox_privatekeys"

#define SB_MAXIMUM_PATH_LENGTH 128
#define SB_MAXIMUM_FILE_PACKAGE_SIZE 4096

// basic structs
// TODO What if I have multiple publishers? Nodes must have a way to query the correct host keypair...
struct node_t {
  std::string  endpoint;
  int          sb_subtype;
  int64_t      last_timestamp;
  int16_t      offset;
  std::string  public_key;
  std::string  uid;
};
struct host_t {
  std::string           endpoint;
  zmqpp::curve::keypair keypair;
  std::string           uid;
};
struct box_t {
  std::string uid;
  std::string base_path;
};

// wrapper for polling on one socket while simultaneously polling the broadcast
void s_recv(zmqpp::socket &socket, zmqpp::socket &broadcast, std::stringstream &sstream);
void s_recv(zmqpp::socket &socket, zmqpp::socket &broadcast, zmqpp::socket &heartbeat, std::stringstream &sstream);
// wrapper for polling on one socket while simultaneously polling the broadcast, but non-blocking
int s_recv_noblock(zmqpp::socket &socket, zmqpp::socket &socket2, zmqpp::socket &broadcast, std::stringstream &sstream, int timeout);
// wrapper for polling on inotify event while simultaneously polling the broadcast
void s_recv_in(zmqpp::socket &broadcast, zmqpp::socket &socket, int fd, std::stringstream &sstream);

#endif