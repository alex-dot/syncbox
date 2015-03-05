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
#include <zmq.hpp>

#include "constants.hpp"
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

    int setContext(zmq::context_t* z_ctx)
    {
      z_ctx_ = z_ctx;
      if ( z_ctx_ == nullptr )
        return 0;
      else
        return 1;

      return -1;
    }
    int connectToBroadcast();
    int connectToBoxoffice();
    int sendExitSignal();

    int watch();

    void recursivePrint() const;
    void printDirectories() const;

  private:
    void recursiveDirectoryFill(std::vector<Hash*>& hashes, std::vector<boost::filesystem::directory_entry>* dir);

    zmq::context_t* z_ctx_;
    zmq::socket_t* z_broadcast_;
    zmq::socket_t* z_boxoffice_;
    boost::filesystem::path path_;
    std::unordered_map<std::string,Directory*> entries_;
    HashTree* hash_tree_;
    std::vector<int> watch_descriptors_;
};

#endif