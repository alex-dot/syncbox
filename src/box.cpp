/*
 * Box
 *
 */

#include "box.hpp"

#include <boost/filesystem.hpp>
#include <vector>

#include "directory.hpp"

#include <stdio.h>
#include <iostream>

Box::Box() :
  path_(),
  entries_()
  {}

Box::Box(boost::filesystem::path p) :
  path_(p),
  entries_()
  {
    Directory* baseDir = new Directory(p);
    entries_.push_back(baseDir);
    // fill base and iterate over found subdirs
    recursiveDirectoryFill(baseDir->fillDirectory(path_));

    HashTree* temp_ht = new HashTree();
    temp_ht->makeHashTree(entries_);
    hash_tree_ = temp_ht;
  }

void Box::recursiveDirectoryFill(std::vector<boost::filesystem::directory_entry>* dirs)
{
  for ( std::vector<boost::filesystem::directory_entry>::iterator i = dirs->begin();
        i != dirs->end();
        ++i )
  {
    Directory* directory = new Directory(*i);
    entries_.push_back(directory);
    recursiveDirectoryFill(directory->fillDirectory(*i));
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


void Box::recursivePrint() const
{
  for ( std::vector<Directory*>::const_iterator i = entries_.begin();
        i != entries_.end();
        ++i )
  {
    (*i)->printEntries();
  }
}

void Box::printDirectories() const
{
  for ( std::vector<Directory*>::const_iterator i = entries_.begin();
        i != entries_.end();
        ++i )
  {
    std::cout << (*i)->getPath() << std::endl;
  }
}