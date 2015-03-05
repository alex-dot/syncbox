/*
 * Box
 *
 */

#include "box.hpp"

#include <boost/filesystem.hpp>
#include <vector>

#include "constants.hpp"
#include "directory.hpp"

#include <stdio.h>
#include <iostream>

Box::Box() :
  z_ctx_(nullptr),
  z_broadcast_(nullptr),
  z_boxoffice_(nullptr),
  path_(),
  entries_(),
  hash_tree_()
  {}

Box::Box(boost::filesystem::path p) :
  z_ctx_(nullptr),
  z_broadcast_(nullptr),
  z_boxoffice_(nullptr),
  path_(p),
  entries_(),
  hash_tree_()
  {
    Directory* baseDir = new Directory(p);
    std::vector<Hash*> hashes;

    Hash* hash = new Hash();
    baseDir->makeDirectoryHash(hash);
    entries_[hash->getHash()] = baseDir;
    hashes.push_back(hash);

    recursiveDirectoryFill(hashes, baseDir->fillDirectory(path_));

    HashTree* temp_ht = new HashTree();
    temp_ht->makeHashTree(hashes);
    hash_tree_ = temp_ht;
  }

void Box::recursiveDirectoryFill(std::vector<Hash*>& hashes, std::vector<boost::filesystem::directory_entry>* dirs)
{
  for ( std::vector<boost::filesystem::directory_entry>::iterator i = dirs->begin();
        i != dirs->end();
        ++i )
  {
    Directory* directory = new Directory(*i);
    Hash* hash = new Hash();
    directory->makeDirectoryHash(hash);
    entries_[hash->getHash()] = directory;
    hashes.push_back(hash);
    recursiveDirectoryFill(hashes, directory->fillDirectory(*i));
  }
}

HashTree* Box::getHashTree() const { return hash_tree_; }
bool Box::checkBoxChange(const Box& left) const
{
  return (left.getHashTree()->getTopHash() == hash_tree_->getTopHash()) ? false : true;
}
bool Box::getChangedDirHashes(std::vector<Hash*>& changed_hashes, 
                              const Box& left) const
{
  return hash_tree_->getChangedHashes(changed_hashes, *(left.getHashTree()));
}

int Box::connectToBroadcast()
{
  if (SB_MSG_DEBUG) printf("box: connecting to broadcast\n");
  z_broadcast_ = new zmq::socket_t(*z_ctx_, ZMQ_SUB);
  z_broadcast_->connect("inproc://sb_broadcast");
  const char *sub_filter = "";
  z_broadcast_->setsockopt(ZMQ_SUBSCRIBE, sub_filter, 0);

  return 0;
}
int Box::connectToBoxoffice()
{
  z_boxoffice_ = new zmq::socket_t(*z_ctx_, ZMQ_PUSH);
  z_boxoffice_->connect("inproc://sb_boxoffice_pull_in");

  // send a heartbeat to boxoffice, so it knows the box is ready
  if (SB_MSG_DEBUG) printf("box: sending heartbeat...\n");
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_ALIVE);
  z_boxoffice_->send(z_msg);

  return 0;
}

int Box::sendExitSignal()
{
  // send exit signal to boxoffice
  if (SB_MSG_DEBUG) printf("box: sending exit signal...\n");
  zmq::message_t z_msg;
  snprintf((char*) z_msg.data(), 4, "%d %d", SB_SIGTYPE_LIFE, SB_SIGLIFE_EXIT);
  z_boxoffice_->send(z_msg);

  if (SB_MSG_DEBUG) printf("box: signal sent, exiting...\n");
  z_boxoffice_->close();

  z_broadcast_->close();

  return 0;
}

int Box::watch()
{
  // defining fd 
  int fd, wd;
  char buffer[SB_IN_BUF_LEN];

  fd = inotify_init();
  if ( fd < 0 ) return 1;

  // for each directory, add a watch
  watch_descriptors_.reserve(entries_.size());
  for (std::unordered_map<std::string,Directory*>::const_iterator i = 
       entries_.begin(); i != entries_.end(); ++i)
  {
    wd = inotify_add_watch( fd, i->second->getAbsolutePath().c_str(), SB_IN_EVENT_MASK );
    watch_descriptors_.push_back(wd);
  }

  std::stringstream* sstream;
  int msg_type, msg_signal;
  while(true)
  {
    sstream = new std::stringstream();
    s_recv_in(*z_broadcast_, fd, *sstream);
    *sstream >> msg_type >> msg_signal;
    if ( msg_type == SB_SIGTYPE_LIFE || msg_signal == SB_SIGLIFE_INTERRUPT ) break;

    // sending inotify event
    std::string message = sstream->str();
    zmq::message_t* z_msg;
    z_msg = new zmq::message_t(5);
    snprintf((char*) z_msg->data(), 5, "%d %d\n", SB_SIGTYPE_INOTIFY, SB_SIGIN_EVENT);
    z_boxoffice_->send(*z_msg, ZMQ_SNDMORE);
    delete z_msg;
    z_msg = new zmq::message_t(message.length()+1);
    snprintf((char*) z_msg->data(), message.length()+1, "%s", message.c_str());
    z_boxoffice_->send(*z_msg);
    delete z_msg;

    delete sstream;
  }

  z_broadcast_->close();

  close(fd);

  return 0;
}


void Box::recursivePrint() const
{
  for ( std::unordered_map<std::string,Directory*>::const_iterator i = entries_.begin();
        i != entries_.end();
        ++i )
  {
    (*i).second->printEntries();
  }
}

void Box::printDirectories() const
{
  for ( std::unordered_map<std::string,Directory*>::const_iterator i = entries_.begin();
        i != entries_.end();
        ++i )
  {
    std::cout << (*i).second->getPath() << std::endl;
  }
}