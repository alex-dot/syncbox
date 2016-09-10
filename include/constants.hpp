/*
 * generic constants and globals for application
 */

#ifndef F_CONSTANTS_HPP
#define F_CONSTANTS_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include <zmqpp/curve.hpp>
#include <signal.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <ctime>
#include <unordered_map>

#include <hash.hpp>

enum F_SIGTYPE {
  F_SIGTYPE_LIFE,
  F_SIGTYPE_PUB,
  F_SIGTYPE_SUB,
  F_SIGTYPE_INOTIFY,
  F_SIGTYPE_FSM
};
enum F_SIGLIFE {
  F_SIGLIFE_ALIVE,
  F_SIGLIFE_EXIT,
  F_SIGLIFE_INTERRUPT,
  F_SIGLIFE_ERROR=-1
};
enum F_SIGPUB {
  F_SIGPUB_GET_CHANNELS
};
enum F_SIGSUB {
  F_SIGSUB_GET_CHANNELS
};
enum F_SUB_TYPE {
  F_SUBTYPE_TCP_BIDIR,
  F_SUBTYPE_TCP_UNIDIR
};
enum F_BACKUP_TYPE {
  F_BACKUP_NONE,
  F_BACKUP_LOCAL,
  F_BACKUP_REMOTE,
  F_BACKUP_BOTH
};

#define F_MSG_DEBUG true

//static size_t  
#define F_IN_EVENT_SIZE (sizeof(struct inotify_event))
//static size_t  
#define F_IN_BUF_LEN    (1024 * (F_IN_EVENT_SIZE + 16))
#define F_IN_EVENT_MASK  IN_ATTRIB|IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_MODIFY|IN_MOVE|IN_MOVE_SELF

#define F_MINIMUM_HB_WIDTH 256
#define F_MINIMUM_SEND_OFFSET 3000
#define F_MAXIMUM_SEND_OFFSET 6000
#define F_MINIMUM_STOP_OFFSET 1000
#define F_MAXIMUM_STOP_OFFSET 2000

#define F_CONFIG_FILE "~/.flocksy"
#define F_KEYSTORE_FILE "~/.ssh/flocksy_keystore"
#define F_PRIVATEKEY_FILE "~/.ssh/flocksy_privatekeys"

#define F_MAXIMUM_PATH_LENGTH 128
#define F_MAXIMUM_FILE_PACKAGE_SIZE 4096

// basic structs
// TODO What if I have multiple publishers? Nodes must have a way to query the correct host keypair...
struct node_t {
  std::string   endpoint;
  int           f_subtype;
  int64_t       last_timestamp;
  int16_t       offset;
  std::string   public_key;
  unsigned char uid[F_GENERIC_HASH_LEN];
};
struct host_t {
  std::string           endpoint;
  zmqpp::curve::keypair keypair;
  std::string           uid;
};
struct box_t {
  unsigned char uid[F_GENERIC_HASH_LEN];
  std::string   base_path;
};

typedef std::unordered_map< Hash*,
                            node_t,
                            hashAsKeyForContainerFunctor,
                            hashPointerEqualsFunctor > node_map;

// wrapper for polling on one socket while simultaneously polling the broadcast
void s_recv(zmqpp::socket &socket, zmqpp::socket &broadcast, std::stringstream &sstream);
void s_recv(zmqpp::socket &socket,
            zmqpp::socket &broadcast,
            zmqpp::socket &heartbeat,
            std::stringstream &sstream);
void s_recv(zmqpp::socket &socket,
            zmqpp::socket &broadcast,
            zmqpp::socket &heartbeat,
            zmqpp::socket &dispatch,
            std::stringstream &sstream);
// wrapper for polling on three sockets, but non-blocking
int s_recv_noblock(zmqpp::socket &socket, zmqpp::socket &socket2, zmqpp::socket &broadcast, std::stringstream &sstream, int timeout);
// wrapper for polling on inotify event while simultaneously polling the broadcast
void s_recv_in(zmqpp::socket &broadcast, zmqpp::socket &socket, int fd, std::stringstream &sstream);

#endif