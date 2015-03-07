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
#include "hash_tree.hpp"

class Directory
{
  public: 
    Directory();
    Directory(const boost::filesystem::path&);
    ~Directory();

    void fillDirectory(const boost::filesystem::path&, 
                       std::vector<boost::filesystem::directory_entry>&);
    void makeDirectoryHash(std::shared_ptr<Hash> hash);

    HashTree* getHashTree() const;

    bool checkDirectoryChange(const Directory& left) const;
    bool getChangedEntryHashes(std::vector< std::shared_ptr<Hash> >& changed_hashes,
                               const Directory& left) const;

    const std::string getPath() const;
    const std::string getAbsolutePath() const;
          int         getNumberOfEntries() const;

    void printHashTree() const;
    void printEntries() const;

  private:
    boost::filesystem::path path_;
    std::unordered_map<std::string,boost::filesystem::directory_entry> entries_;
    HashTree* hash_tree_;
    Hash* directory_hash_;
};

#endif