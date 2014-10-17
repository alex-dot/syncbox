/*
 *
 *
 */

#ifndef SB_HASH_TREE_HPP
#define SB_HASH_TREE_HPP

#include <vector>

#include "hash.hpp"
#include "directory.hpp"
//class Directory;

class HashTree
{
  public:
    HashTree() : hashes_(){}
    // this is actually ambivalent: a vector of Hash-Pointers could
    // either mean a complete HashTree or a list of hashes to be made
    // a HashTree. We therefore stick to say that it is a HashTree
    // and let the programmer call makeHashTreeFromSelf() otherwise.
    HashTree(std::vector<Hash*> hashes) : hashes_(hashes){}
    ~HashTree();

    void makeHashTree(std::vector<Hash*> temp_hashes);
    void makeHashTree(std::vector<Directory*> dirs);
    void makeHashTreeFromSelf();

    std::vector<Hash*> const getHashes() const;
    bool compareHashTree(HashTree& left) const;
    std::vector<Hash*> const getChangedHashes() const;

    std::vector<int> const getElementsPerLevel() const { return elements_per_level_; }

  private:
    std::vector<Hash*> hashes_;
    std::vector<int> elements_per_level_;
};

bool inline compareHashTrees(HashTree& lhs, HashTree& rhs)
{
  if ( lhs.getHashes().back()->getHash() != rhs.getHashes().back()->getHash() )
    return false;
  else 
    return true;
}
bool inline operator==(HashTree& lhs, HashTree& rhs) { return compareHashTrees(lhs, rhs); }
bool inline operator!=(HashTree& lhs, HashTree& rhs) { return !compareHashTrees(lhs, rhs); }


#endif