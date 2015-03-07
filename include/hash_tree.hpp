/*
 *
 *
 */

#ifndef SB_HASH_TREE_HPP
#define SB_HASH_TREE_HPP

#include <vector>
#include <memory>

#include "hash.hpp"

class HashTree
{
  public:
    HashTree() : hashes_(){}
    // this is actually ambivalent: a vector of Hash-Pointers could
    // either mean a complete HashTree or a list of hashes to be made
    // a HashTree. We therefore stick to say that it is a HashTree
    // and let the programmer call makeHashTreeFromSelf() otherwise.
    HashTree(const std::vector< std::shared_ptr<Hash> >& hashes) : hashes_(hashes){}
    ~HashTree();

    void makeHashTree(std::vector< std::shared_ptr<Hash> >& temp_hashes);
    void makeHashTreeFromSelf();

    const std::vector< std::shared_ptr<Hash> >* getHashes() const;
    std::shared_ptr<Hash> getTopHash() const;


    bool checkHashTreeChange(const HashTree& left) const;
    bool getChangedHashes(std::vector< std::shared_ptr<Hash> >& changed_hashes, const HashTree& lhs) const;

    const std::vector<int>* getElementsPerLevel() const { return &elements_per_level_; }

  private:
    std::vector< std::shared_ptr<Hash> > hashes_;
    std::vector<int> elements_per_level_;
};

bool inline checkHashTreeChange(const HashTree& lhs, const HashTree& rhs)
{
  if ( lhs.getTopHash()->getHash() != rhs.getTopHash()->getHash() )
    return false;
  else 
    return true;
}
bool inline operator==(HashTree& lhs, HashTree& rhs) { return checkHashTreeChange(lhs, rhs); }
bool inline operator!=(HashTree& lhs, HashTree& rhs) { return !checkHashTreeChange(lhs, rhs); }


#endif