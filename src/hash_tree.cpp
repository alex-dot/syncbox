/*
 *
 *
 */

#include "hash_tree.hpp"

#include <algorithm>
#include <string>
#include <sstream>

#include <iostream>

HashTree::~HashTree()
{/*
  for (std::vector<Hash*>::iterator i = hashes_.begin(); i != hashes_.end(); ++i)
  {
    if (*i != nullptr)
    {
      delete *i;
      *i = nullptr;
    }
  }*/
}

void HashTree::makeHashTree(std::vector< std::shared_ptr<Hash> >& temp_hashes)
{
  // if this object already has a tree, clean up
  hashes_.clear();
  elements_per_level_.clear();

  std::sort (temp_hashes.begin(), temp_hashes.end(), hashSharedPointerLessThanFunctor());

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

  std::vector< std::shared_ptr<Hash> > hashes;
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
          std::shared_ptr<Hash> hash(new Hash(string));
          hashes.push_back(hash);
          ++temp_node_count;
        } else if ( ((offset+2) - elements_per_level_.back() == 1) ) {
          std::string string = hashes[curr_item]->getHash();
          std::shared_ptr<Hash> hash(new Hash(string+string));
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
void HashTree::makeHashTreeFromSelf()
{
  // we copy the internal hash vector to circumvent race conditions
  std::vector< std::shared_ptr<Hash> > temp_hashes = hashes_;
  this->makeHashTree(temp_hashes);
}

const std::vector< std::shared_ptr<Hash> >* HashTree::getHashes() const { return &hashes_; }
std::shared_ptr<Hash> HashTree::getTopHash() const { return hashes_.back(); }


bool HashTree::checkHashTreeChange(const HashTree& lhs) const
{
  if ( lhs.getTopHash()->getHash() != hashes_.back()->getHash() )
    return true;
  else 
    return false;
}

bool HashTree::getChangedHashes(std::vector< std::shared_ptr<Hash> >& changed_hashes, 
                                const HashTree& lhs) const
{
  if (this->checkHashTreeChange(lhs))
  {
    int max_elements_size = lhs.getElementsPerLevel()->front() + elements_per_level_.front();
    changed_hashes.resize(max_elements_size);

    std::vector< std::shared_ptr<Hash> > left_hashes(lhs.getElementsPerLevel()->front());
    std::vector< std::shared_ptr<Hash> > left_hashes_unique(lhs.getElementsPerLevel()->front());
    std::copy_n (lhs.getHashes()->begin(), lhs.getElementsPerLevel()->front(), left_hashes.begin());
    std::vector< std::shared_ptr<Hash> > right_hashes(elements_per_level_.front());
    std::vector< std::shared_ptr<Hash> > right_hashes_unique(elements_per_level_.front());
    std::copy_n (hashes_.begin(), elements_per_level_.front(), right_hashes.begin());

    std::sort (left_hashes.begin(), left_hashes.end(), hashSharedPointerLessThanFunctor());
    std::sort (right_hashes.begin(), right_hashes.end(), hashSharedPointerLessThanFunctor());

    std::vector< std::shared_ptr<Hash> >::iterator it;
    it = std::unique_copy (left_hashes.begin(), left_hashes.end(), left_hashes_unique.begin(), hashSharedPointerEqualsFunctor());
    left_hashes_unique.resize(std::distance(left_hashes_unique.begin(),it));
    it = std::unique_copy (right_hashes.begin(), right_hashes.end(), right_hashes_unique.begin(), hashSharedPointerEqualsFunctor());
    right_hashes_unique.resize(std::distance(right_hashes_unique.begin(),it));

    it = set_symmetric_difference(left_hashes_unique.begin(), left_hashes_unique.end(), 
                                  right_hashes_unique.begin(), right_hashes_unique.end(), 
                                  changed_hashes.begin(), hashSharedPointerLessThanFunctor());

    changed_hashes.resize(std::distance(changed_hashes.begin(),it));
    return true;
  } else {
    return false;
  }
}