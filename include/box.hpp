/*
 * A Box is a collection of a base directory and its sub-directories
 * contained in an unordered map with the hash of a directory as the key. 
 * The Box gets defined and added by the user. It is the base files
 * class through which syncbox handles all files in a given directory.
 *
 * 
 */

#ifndef SB_BOX_HPP
#define SB_BOX_HPP

#include <boost/filesystem.hpp>
#include <vector>
#include <unordered_map>
#include <zmqpp/zmqpp.hpp>

#include "constants.hpp"
#include "transmitter.hpp"
#include "directory.hpp"
#include "hash_tree.hpp"

class Box : public Transmitter
{
  public:
    Box();
    Box(zmqpp::context* z_ctx_, boost::filesystem::path, Hash* box_hash);
    ~Box();

    HashTree* getHashTree() const;

    bool checkBoxChange(const Box& left) const;
    bool getChangedDirHashes(std::vector< std::shared_ptr<Hash> >& changed_hashes, 
                             const Box& left) const;

    int run();

    const std::string getBaseDir() const;
    const std::string getPathOfDirectory(int wd) const;
    const std::string getAbsolutePathOfDirectory(int wd) const;

    void recursivePrint() const;
    void printDirectories() const;

  private:
    void recursiveDirectoryFill(std::vector< std::shared_ptr<Hash> >& hashes, 
                                std::vector<boost::filesystem::directory_entry>& dir);

    boost::filesystem::path                    path_;
    std::unordered_map<std::string,Directory*> entries_;
    HashTree*                                  hash_tree_;
    std::unordered_map<int,Directory*>         watch_descriptors_;
    Hash*                                      box_hash_;
};

#endif