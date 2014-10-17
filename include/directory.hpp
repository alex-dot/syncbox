/*
 *
 *
 */

#ifndef SB_DIRS_HPP
#define SB_DIRS_HPP
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem.hpp>
#include <vector>
#include <unordered_map>
#include <string>

#include "hash.hpp"

class HashTree; // forward declaration

class Directory
{
  public: 
    Directory();
    Directory(const boost::filesystem::path&);
    ~Directory();

    std::vector<boost::filesystem::directory_entry>* fillDirectory(const boost::filesystem::path&);

    HashTree* getHashTree() const { return hash_tree_; }

    void printPath() const;
    void printHashTree() const;
    void printEntries() const;

  private:
    boost::filesystem::path path_;
    std::map<Hash*,boost::filesystem::directory_entry> entries_;
    HashTree* hash_tree_;
    Hash* directory_hash_;
};

#endif