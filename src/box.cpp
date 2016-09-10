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
  Transmitter(),
  path_(),
  entries_(),
  hash_tree_(),
  box_hash_()
  {}

Box::Box(zmqpp::context* z_ctx_,
         boost::filesystem::path p,
         const unsigned char box_hash[F_GENERIC_HASH_LEN]) :
  Transmitter(z_ctx_),
  path_(p),
  entries_(),
  hash_tree_(),
  box_hash_(new unsigned char[F_GENERIC_HASH_LEN])
  {
    tac = (char*)"box";
    Directory* baseDir = new Directory(p);
    std::vector< std::shared_ptr<Hash> >* hashes = new std::vector< std::shared_ptr<Hash> >;

    std::shared_ptr<Hash> hash(new Hash());
    baseDir->makeDirectoryHash(hash);
    entries_[hash->getString()] = baseDir;
    hashes->push_back(hash);

    std::vector<boost::filesystem::directory_entry> dirs;
    baseDir->fillDirectory(path_, dirs);
    recursiveDirectoryFill(hashes, dirs);

    HashTree* temp_ht = new HashTree();
    temp_ht->makeHashTree(*hashes);
    std::swap(hash_tree_,temp_ht);
    delete temp_ht;

    std::memcpy(box_hash_, box_hash, F_GENERIC_HASH_LEN);
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
}

void Box::recursiveDirectoryFill(
  std::vector< std::shared_ptr<Hash> >* hashes,
  const std::vector<boost::filesystem::directory_entry>& dirs)
{
  for ( std::vector<boost::filesystem::directory_entry>::const_iterator i = 
        dirs.begin(); i != dirs.end();
        ++i )
  {
    Directory* directory = new Directory(*i);
    std::shared_ptr<Hash> hash(new Hash());
    directory->makeDirectoryHash(hash);
    entries_.insert(std::make_pair(hash->getString(),directory));
    hashes->push_back(hash);
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
bool Box::getChangedDirHashes(std::vector< std::shared_ptr<Hash> >* changed_hashes, 
                              const Box& left) const
{
  return hash_tree_->getChangedHashes(*changed_hashes, *(left.getHashTree()));
}

int Box::run()
{
  // defining fd 
  int fd, wd;

  fd = inotify_init();
  if ( fd < 0 ) return 1;

  // for each directory, add a watch
  for (std::unordered_map<std::string,Directory*>::iterator i = 
       entries_.begin(); i != entries_.end(); ++i)
  {
    wd = inotify_add_watch( fd, i->second->getAbsolutePath().c_str(), F_IN_EVENT_MASK );
    watch_descriptors_.insert(std::make_pair(wd,i->second));
  }

  std::stringstream* sstream;
  int msg_type, msg_signal;
  while(true)
  {
    sstream = new std::stringstream();
    s_recv_in(*z_broadcast, *z_boxoffice_push, fd, *sstream);
    *sstream >> msg_type >> msg_signal;
    if ( msg_type == F_SIGTYPE_LIFE && msg_signal == F_SIGLIFE_INTERRUPT ) 
    {
      delete sstream;
      break;
    }

    // sending inotify event
    int wd;
    std::string dir_path, name;
    *sstream >> wd >> name;
    dir_path = getPathOfDirectory(wd);

    int inotify_mask = msg_signal;
    if ((inotify_mask & IN_DELETE_SELF) != IN_DELETE_SELF) {
      fsm::status_t status;
      if (        ((inotify_mask & IN_MODIFY)      == IN_MODIFY)
               || ((inotify_mask & IN_MOVE)        == IN_MOVE)
               || ((inotify_mask & IN_MOVE_SELF)   == IN_MOVE_SELF) ) {
        // fsm::new_local_file_event;
        status = fsm::status_300;
      } else if ( ((inotify_mask & IN_CREATE)      == IN_CREATE)
               || ((inotify_mask & IN_ATTRIB)      == IN_ATTRIB)
               || ((inotify_mask & IN_DELETE)      == IN_DELETE) ) {
        // fsm::local_file_metadata_change_event;
        status = fsm::status_320;
      }

      std::stringstream message;
      message << F_SIGTYPE_INOTIFY << " "
              << status             << " "
              << inotify_mask;
      message.write(reinterpret_cast<char*>(box_hash_), F_GENERIC_HASH_LEN);
      message << " " << dir_path    << "/"
              << name.c_str();

      zmqpp::message* z_msg = new zmqpp::message();
      *z_msg << message.str();
      z_boxoffice_pull->send(*z_msg);
      delete z_msg;
    }

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
  path_string = path_string.substr(this->getBaseDir().length());
  return path_string;
}
const std::string Box::getAbsolutePathOfDirectory(int wd) const
  { return watch_descriptors_.at(wd)->getAbsolutePath(); }
const unsigned char* Box::getBoxHash() const {
  return box_hash_;
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