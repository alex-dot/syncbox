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
#include <utility>
#include <memory>

#include <stdio.h>
#include <string>
#include <cmath>

Directory::Directory() :
  path_(),
  entries_(),
  hash_tree_(),
  directory_hash_()
  {}

Directory::Directory(const boost::filesystem::path& p) :
  path_(p),
  entries_(),
  hash_tree_(),
  directory_hash_()
  {
    std::vector<boost::filesystem::directory_entry> dirs;
    fillDirectory(path_, dirs);
  }

Directory::~Directory()
{
  //delete hashes
  delete hash_tree_;
}

void Directory::fillDirectory(const boost::filesystem::path& document_root, 
                              std::vector<boost::filesystem::directory_entry>& dirs)
{
  // TODO: need EXCEPTION handling for empty hash
  std::vector< std::shared_ptr<Hash> > temp_hashes;
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
      std::shared_ptr<Hash> hash(new Hash(string_to_hash));
      temp_hashes.push_back(hash);

      // insert into entries_
      entries_.insert(std::make_pair(hash->getHash(),*i));
    }

    else
      std::cout << "Directory: error during directory_iterator: " << *i << std::endl;
  }
  HashTree* temp_ht = new HashTree();
  temp_ht->makeHashTree(temp_hashes);
  std::swap(hash_tree_,temp_ht);
  delete temp_ht;
}
void Directory::makeDirectoryHash(std::shared_ptr<Hash> hash)
{
  std::string hash_string = hash_tree_->getTopHash()->getHash();
  hash_string += this->getPath();
  hash->makeHash(hash_string);
}

HashTree* Directory::getHashTree() const { return hash_tree_; }
bool Directory::checkDirectoryChange(const Directory& left) const
{
  return (left.getHashTree() == this->getHashTree()) ? false : true;
}
bool Directory::getChangedEntryHashes(std::vector< std::shared_ptr<Hash> >& changed_hashes,
                       const Directory& left) const
{
  return hash_tree_->getChangedHashes(changed_hashes, *(left.getHashTree()));
}

const std::string Directory::getPath() const { return path_.filename().c_str(); }
const std::string Directory::getAbsolutePath() const { return path_.c_str(); }
      int         Directory::getNumberOfEntries() const { return entries_.size(); }

void Directory::printHashTree() const
{
  std::vector< std::shared_ptr<Hash> > hashes = *(hash_tree_->getHashes());
  for (std::vector< std::shared_ptr<Hash> >::iterator i = hashes.begin(); i != hashes.end(); ++i)
  {
    std::cout << (*i)->getHash() << std::endl;
  }
}
void Directory::printEntries() const
{
  std::cout << getPath() << std::endl;
  for ( std::unordered_map<std::string, boost::filesystem::directory_entry>::const_iterator i = entries_.begin();
        i != entries_.end();
        ++i )
  {
    std::cout << i->second << std::endl;
    std::cout << i->first << std::endl;
    std::cout << i->second.status().permissions() << std::endl;
  }
}