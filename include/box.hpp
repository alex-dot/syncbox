/*
 * A Box is a collection of a base directory and its sub-directories
 * contained in a vector with the base dir as the first vector item. 
 * Iterating over the vector is a pre-order depth-first tree traversal.
 * The Box gets defined and added by the user. It is the base files
 * class through which sync-box handles all files in a given directory.
 */

#ifndef SB_BOX_HPP
#define SB_BOX_HPP

#include <boost/filesystem.hpp>
#include <vector>
#include <unordered_map>

#include "directory.hpp"
#include "hash_tree.hpp"

class Box
{
  public:
    Box();
    Box(boost::filesystem::path);
    ~Box() {}

    HashTree* getHashTree() const;

    bool checkBoxChange(const Box& left) const;
    bool getChangedDirHashes(std::vector<Hash*>& changed_hashes, 
                             const Box& left) const;

    void recursivePrint() const;
    void printDirectories() const;

  private:
    void recursiveDirectoryFill(std::vector<Hash*>& hashes, std::vector<boost::filesystem::directory_entry>* dir);

    boost::filesystem::path path_;
    std::unordered_map<std::string,Directory*> entries_;
    HashTree* hash_tree_;
};

#endif