/*
 * Directory
 *
 */

#include "directory.hpp"
#include "hash.hpp"
#include "hash_tree.hpp"

#include <boost/filesystem.hpp>
#include <vector>
#include <unordered_map>

#include <stdio.h>
#include <string>
#include <cmath>

Directory::Directory() :
  path_(),
  entries_(),
  hash_tree_(),
  directory_hash_()
  {}

Directory::Directory(boost::filesystem::path p) :
  path_(p),
  entries_(),
  hash_tree_(),
  directory_hash_()
  {
    fillDirectory(path_);
  }

Directory::~Directory()
{
  //delete hashes
}

std::vector<boost::filesystem::directory_entry> Directory::fillDirectory(boost::filesystem::path document_root)
{
  std::vector<boost::filesystem::directory_entry> dirs;
  std::vector<Hash*> temp_hashes;
  // iterate over the given path and write every file to entries_, return directories
  for ( boost::filesystem::directory_iterator i = boost::filesystem::directory_iterator(document_root); 
        i != boost::filesystem::directory_iterator(); 
        ++i )
  {
    if (is_directory(*i))
      dirs.push_back(*i);

    else if (is_regular_file(*i))
    {
      std::string string_to_hash = "";

      // add filename to string
      boost::filesystem::path file = i->path();
      std::string filename = file.filename().c_str();
      string_to_hash += filename;

      // add file path relative to document_root to string
      int document_root_length = strlen(document_root.c_str());
      std::string relative_file_path = file.c_str();
      relative_file_path = relative_file_path.substr(document_root_length,
                                                     (strlen(file.c_str())-filename.length())-document_root_length);
      string_to_hash += relative_file_path;

      // add timestamp to string
      string_to_hash += std::to_string(boost::filesystem::last_write_time(file));

      // make hash
      Hash* hash = new Hash(string_to_hash);
      temp_hashes.push_back(hash);

      // insert into entries_
      entries_[hash] = *i;
    }

    else
      std::cout << "Directory: error during directory_iterator: " << *i << std::endl;
  }
  HashTree* temp_ht = new HashTree();
  temp_ht->makeHashTree(temp_hashes);
  hash_tree_ = temp_ht;
  return dirs;
}

void Directory::printPath() const
{
  std::cout << path_.filename() << std::endl;
}
void Directory::printHashTree() const
{
  std::vector<Hash*> hash_tree = hash_tree_->getHashes();
  for (std::vector<Hash*>::iterator i = hash_tree.begin(); i != hash_tree.end(); ++i)
  {
    std::cout << (*i)->getHash() << std::endl;
  }
}
void Directory::printEntries() const
{
  printPath();
  for ( std::map<Hash*, boost::filesystem::directory_entry>::const_iterator i = entries_.begin();
        i != entries_.end();
        ++i )
  {
    std::cout << i->second << std::endl;
    std::cout << i->first->getHash() << std::endl;
    std::cout << i->second.status().permissions() << std::endl;
  }
}