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
  entries_(),
  hash_tree_()
  {}

Box::Box(boost::filesystem::path p) :
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