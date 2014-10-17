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
    std::vector<boost::filesystem::directory_entry>* dirs;
    Directory* baseDir = new Directory(p);
    entries_.push_back(baseDir);
    // fill base and iterate over found subdirs
    dirs = baseDir->fillDirectory(path_);
    recursiveDirectoryFill(dirs);
    delete dirs;

    HashTree* temp_ht = new HashTree();
    temp_ht->makeHashTree(entries_);
    hash_tree_ = temp_ht;
  }

Box::~Box()
{
  for ( std::vector<Directory*>::iterator i = entries_.begin();
        i != entries_.end();
        ++i )
  {
    delete *i;
  }
}

void Box::recursiveDirectoryFill(std::vector<boost::filesystem::directory_entry>* dirs)
{
  for ( std::vector<boost::filesystem::directory_entry>::iterator i = dirs->begin();
        i != dirs->end();
        ++i )
  {
    Directory* directory = new Directory(*i);
    entries_.push_back(directory);
    std::vector<boost::filesystem::directory_entry>* sub_dirs;
    sub_dirs = directory->fillDirectory(*i);
    recursiveDirectoryFill(sub_dirs);
    delete sub_dirs;
  }
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
    (*i)->printPath();
  }
}