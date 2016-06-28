/*
 * Box
 *
 */

#include "box.hpp"

#include <boost/filesystem.hpp>
#include <vector>
#include <utility>

#include "constants.hpp"
#include "directory.hpp"

#include <stdio.h>
#include <iostream>

namespace fsm {
  #include "tarmuft_fsm.h"
}

Box::Box() :
  z_ctx_(nullptr),
  z_broadcast_(nullptr),
  z_boxoffice_pull(nullptr),
  z_boxoffice_push(nullptr),
  path_(),
  entries_(),
  hash_tree_(),
  box_num_()
  {}

Box::Box(boost::filesystem::path p, int box_num) :
  z_ctx_(nullptr),
  z_broadcast_(nullptr),
  z_boxoffice_pull(nullptr),
  z_boxoffice_push(nullptr),
  path_(p),
  entries_(),
  hash_tree_(),
  box_num_(box_num)
  {
    Directory* baseDir = new Directory(p);
    std::vector< std::shared_ptr<Hash> > hashes;

    std::shared_ptr<Hash> hash(new Hash());
    baseDir->makeDirectoryHash(hash);
    entries_[hash->getHash()] = baseDir;
    hashes.push_back(hash);

    std::vector<boost::filesystem::directory_entry> dirs;
    baseDir->fillDirectory(path_, dirs);
    recursiveDirectoryFill(hashes, dirs);

    HashTree* temp_ht = new HashTree();
    temp_ht->makeHashTree(hashes);
    std::swap(hash_tree_,temp_ht);
    delete temp_ht;
  }

Box::~Box()
{
  // note: we only would need to do this for entries_, since we reference the 
  // same Directories* in watch_descriptors_
  // but let's make sure and clear both unordered_maps regardless
  for (std::unordered_map<std::string,Directory*>::iterator i = entries_.begin(); i != entries_.end(); ++i)
    delete i->second;
  entries_.clear();
  watch_descriptors_.clear();

  delete hash_tree_;
  delete z_broadcast_;
  delete z_boxoffice_pull;
  delete z_boxoffice_push;
}

void Box::recursiveDirectoryFill(std::vector< std::shared_ptr<Hash> >& hashes, std::vector<boost::filesystem::directory_entry>& dirs)
{
  for ( std::vector<boost::filesystem::directory_entry>::iterator i = 
        dirs.begin(); i != dirs.end();
        ++i )
  {
    Directory* directory = new Directory(*i);
    std::shared_ptr<Hash> hash(new Hash());
    directory->makeDirectoryHash(hash);
    entries_.insert(std::make_pair(hash->getHash(),directory));
    hashes.push_back(hash);
    std::vector<boost::filesystem::directory_entry> temp_dirs;
    directory->fillDirectory(*i, temp_dirs);
    recursiveDirectoryFill(hashes, temp_dirs);
  }
}

HashTree* Box::getHashTree() const { return hash_tree_; }
bool Box::checkBoxChange(const Box& left) const
{
  return (left.getHashTree()->getTopHash() == hash_tree_->getTopHash()) ? false : true;
}
bool Box::getChangedDirHashes(std::vector< std::shared_ptr<Hash> >& changed_hashes, 
                              const Box& left) const
{
  return hash_tree_->getChangedHashes(changed_hashes, *(left.getHashTree()));
}

int Box::connectToBroadcast()
{
  if (SB_MSG_DEBUG) printf("box: connecting to broadcast\n");
  z_broadcast_ = new zmqpp::socket(*z_ctx_, zmqpp::socket_type::sub);
  z_broadcast_->connect("inproc://sb_broadcast");
  z_broadcast_->subscribe("");

  return 0;
}
int Box::connectToBoxoffice()
{
  // open connection to send data to boxoffice
  z_boxoffice_pull = new zmqpp::socket(*z_ctx_, zmqpp::socket_type::push);
  z_boxoffice_pull->connect("inproc://sb_boxoffice_pull_in");
  // open connection to receive data from boxoffice
  z_boxoffice_push = new zmqpp::socket(*z_ctx_, zmqpp::socket_type::sub);
  z_boxoffice_push->connect("inproc://sb_boxoffice_push_out");
  z_boxoffice_push->subscribe("");

  // send a heartbeat to boxoffice, so it knows the box is ready
  if (SB_MSG_DEBUG) printf("box: sending heartbeat...\n");
  std::stringstream message;
  message << SB_SIGTYPE_LIFE << " " << SB_SIGLIFE_ALIVE;
  zmqpp::message z_msg;
  z_msg << message.str();
  z_boxoffice_pull->send(z_msg);

  return 0;
}

int Box::sendExitSignal()
{
  // send exit signal to boxoffice
  if (SB_MSG_DEBUG) printf("box: sending exit signal...\n");
  std::stringstream message;
  message << SB_SIGTYPE_LIFE << " " << SB_SIGLIFE_EXIT;
  zmqpp::message z_msg;
  z_msg << message.str();
  z_boxoffice_pull->send(z_msg);

  if (SB_MSG_DEBUG) printf("box: signal sent, exiting...\n");
  z_boxoffice_pull->close();
  z_boxoffice_push->close();
  z_broadcast_->close();

  return 0;
}

int Box::watch()
{
  // defining fd 
  int fd, wd;

  fd = inotify_init();
  if ( fd < 0 ) return 1;

  // for each directory, add a watch
  for (std::unordered_map<std::string,Directory*>::iterator i = 
       entries_.begin(); i != entries_.end(); ++i)
  {
    wd = inotify_add_watch( fd, i->second->getAbsolutePath().c_str(), SB_IN_EVENT_MASK );
    watch_descriptors_.insert(std::make_pair(wd,i->second));
  }

  std::stringstream* sstream;
  int msg_type, msg_signal;
  while(true)
  {
    sstream = new std::stringstream();
    s_recv_in(*z_broadcast_, *z_boxoffice_push, fd, *sstream);
    *sstream >> msg_type >> msg_signal;
    if ( msg_type == SB_SIGTYPE_LIFE && msg_signal == SB_SIGLIFE_INTERRUPT ) 
    {
      delete sstream;
      break;
    }

    // sending inotify event
    std::string infomessage = sstream->str();
    std::stringstream message;
    message << SB_SIGTYPE_INOTIFY << " "
            << fsm::status_300    << " "
            << box_num_;
    zmqpp::message* z_msg = new zmqpp::message();
    *z_msg << message.str();
    z_boxoffice_pull->send(*z_msg, ZMQ_SNDMORE);
    delete z_msg;

    message << infomessage.c_str();
    z_msg = new zmqpp::message();
    *z_msg << message.str();
    z_boxoffice_pull->send(*z_msg);
    delete z_msg;

    delete sstream;
  }

  close(fd);

  return 0;
}

const std::string Box::getBaseDir() const
  { return path_.c_str(); }
const std::string Box::getPathOfDirectory(int wd) const
{
  Directory* dir = watch_descriptors_.at(wd);
  std::string path_string = dir->getAbsolutePath();
//  std::cout << "new path" << std::endl;
//  std::cout << path_string.length() << std::endl;
//  std::cout << path_string << std::endl;
  path_string = path_string.substr(this->getBaseDir().length());
//  std::cout << path_string.length() << std::endl;
//  std::cout << path_string << std::endl;
//  std::cout << this->getBaseDir() << std::endl;
//  std::cout << this->getBaseDir().length() << std::endl << std::endl;
  return path_string;
}
const std::string Box::getAbsolutePathOfDirectory(int wd) const
  { return watch_descriptors_.at(wd)->getAbsolutePath(); }

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