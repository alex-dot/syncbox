/**
 * \file      box.hpp
 * \brief     A Box is the base files class, watching and organising files. 
 *
 *  A Box is a collection of a base directory and its sub-directories
 *  contained in an unordered map with the hash of a directory as the key. 
 *  The Box gets defined and added by the user. It is the base files
 *  class through which syncbox handles all files in a given directory.
 *
 * \todo      create new wd on newly created directories;
 *            subdirectory expansion
 *
 * \author    Alexander Herr
 * \date      2016
 * \copyright GNU Public License v3 or higher. 
 */

#ifndef INCLUDE_BOX_HPP_
#define INCLUDE_BOX_HPP_

#include <vector>
#include <string>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <zmqpp/zmqpp.hpp>

#include "constants.hpp"
#include "transmitter.hpp"
#include "directory.hpp"
#include "hash_tree.hpp"

class Box : public Transmitter {
 public:
    Box();
    Box(zmqpp::context* z_ctx_,
        boost::filesystem::path,
        const unsigned char box_hash[SB_GENERIC_HASH_LEN]);
    ~Box();

    HashTree* getHashTree() const;

    bool checkBoxChange(const Box& left) const;
    bool getChangedDirHashes(
        std::vector< std::shared_ptr<Hash> >* changed_hashes,
        const Box& left) const;

    int run();

    const std::string getBaseDir() const;
    const std::string getPathOfDirectory(int wd) const;
    const std::string getAbsolutePathOfDirectory(int wd) const;
    const unsigned char* getBoxHash() const;

    void recursivePrint() const;
    void printDirectories() const;

 private:
    void recursiveDirectoryFill(
        std::vector< std::shared_ptr<Hash> >* hashes,
        const std::vector<boost::filesystem::directory_entry>& dir);

    boost::filesystem::path                     path_;
    std::unordered_map<std::string, Directory*> entries_;
    HashTree*                                   hash_tree_;
    std::unordered_map<int, Directory*>         watch_descriptors_;
    unsigned char*                              box_hash_;
};

typedef std::unordered_map< Hash*,
                            Box*,
                            hashAsKeyForContainerFunctor,
                            hashPointerEqualsFunctor > box_map;

#endif  // INCLUDE_BOX_HPP_
