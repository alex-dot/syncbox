/*
 *
 *
 */

#include "hash_tree.hpp"

#include <string>
#include <sstream>

void HashTree::makeHashTree(const std::vector<Hash*>& temp_hashes)
{
  // if this object already has a tree, clean up
  hashes_.clear();
  elements_per_level_.clear();

  int hash_count = temp_hashes.size();
  int tree_depth = 0;                     // the depth of the tree
  int tree_leaf_count = 1;                // the amount of leaf-nodes of the tree
  int tree_size = 1;                      // the complete amount of nodes
                                          // not counting empty leaves!
  // this needs to be exception handled
  // e.g. directories with more than 65535 files will fail
  // this check is optimised so that the result gets
  // rounded up: http://stackoverflow.com/a/2745086/876584
  while ( (1 + ((hash_count - 1) / tree_leaf_count)) > 1 )
  {
    tree_leaf_count = tree_leaf_count << 1;  // this is the same as 2^{++i}
    tree_size += tree_leaf_count;
    ++tree_depth;
  }

  std::vector<Hash*> hashes;
  hashes.reserve(tree_size+1);
  int node_count = 0;
  for ( int i = tree_depth; i >= 0; --i )
  {
    int temp_node_count = 0;
    // on the lowest level, copy the element hashes
    if ( i == tree_depth )
    {
      for (int j = 0; j < hash_count; ++j)
      {
        hashes.push_back(temp_hashes[j]);
      }
      temp_node_count = hash_count;
    } else {
      // for each higher level, create hashes of two lower nodes
      for (int j = 0; j < (1 << i); ++j)
      { 
        int offset = 2 * j;
        int curr_item = node_count + offset;
        // if we have an odd number of lower nodes, simply double the left hash
        if ( (elements_per_level_.back() - (offset+2)) >= 0 )
        {
          std::string string = hashes[curr_item]->getHash();
          string += hashes[curr_item+1]->getHash();
          Hash* hash = new Hash(string);
          hashes.push_back(hash);
          ++temp_node_count;
        } else if ( ((offset+2) - elements_per_level_.back() == 1) ) {
          std::string string = hashes[curr_item]->getHash();
          Hash* hash = new Hash(string+string);
          hashes.push_back(hash);
          ++temp_node_count;
        }
      }
      node_count += elements_per_level_.back();
    }
    elements_per_level_.push_back(temp_node_count);
  }
  hashes.shrink_to_fit();
  hashes_.swap(hashes);
}
void HashTree::makeHashTree(const std::vector<Directory*>& dirs)
{
  std::vector<Hash*> hashes;
  hashes.reserve(dirs.size());
  for (std::vector<Directory*>::const_iterator i = dirs.begin(); i != dirs.end(); ++i)
  {
    Hash* hash = (*i)->getHashTree()->getHashes()->back();
    // need to change this to include the dir's path and mode
    hashes.push_back(hash);
  }
  this->makeHashTree(hashes);
}
void HashTree::makeHashTreeFromSelf()
{
  // we copy the internal hash vector to circumvent race conditions
  std::vector<Hash*> temp_hashes = hashes_;
  this->makeHashTree(temp_hashes);
}

const std::vector<Hash*>* HashTree::getHashes() const { return &hashes_; }

bool HashTree::checkHashTreeChange(const HashTree& lhs) const
{
  if ( lhs.getHashes()->back()->getHash() != hashes_.back()->getHash() )
    return true;
  else 
    return false;
}

bool HashTree::getChangedHashes(std::vector<Hash*>& changed_hashes, 
                                const HashTree& lhs) const
{
  if (this->checkHashTreeChange(lhs))
  {
    changed_hashes.reserve(elements_per_level_[0]);
    std::vector<Hash*> left_hashes = *(lhs.getHashes());
    for (int i = 0; i < elements_per_level_[0]; ++i)
    {
      if (left_hashes[i]->getHash() != hashes_[i]->getHash())
      {
        changed_hashes.push_back(hashes_[i]);
      }
    }
    changed_hashes.shrink_to_fit();
    return true;
  } else {
    return false;
  }
}