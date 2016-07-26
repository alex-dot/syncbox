#include "hash_tree.hpp"

bool hash_tree_compare_helper(unsigned int x)
{
  bool return_value = true;
  std::shared_ptr<Hash> hash_orig(new Hash("test"));
  std::shared_ptr<Hash> hash_diff(new Hash("test2"));
  std::vector< std::shared_ptr<Hash> > hashes_orig(x);
  std::vector< std::shared_ptr<Hash> > hashes_diff(x);
  std::vector< std::shared_ptr<Hash> > hashes_temp;
  std::vector<int> elp;
  HashTree* ht_orig = new HashTree();
  HashTree* ht_diff = new HashTree();
  
  for (std::vector< std::shared_ptr<Hash> >::iterator i = hashes_orig.begin(); i != hashes_orig.end(); ++i)
  { (*i) = hash_orig; }
  ht_orig->makeHashTree(hashes_orig);
  hashes_orig.clear();
  hashes_orig = *(ht_orig->getHashes());
  int rounds = (x>100) ? 100 : x;
  std::vector<int> list_of_culprits(10000);   // a list of differing hashes
  for (int i = 0; i < rounds; ++i)
  {
    for (std::vector< std::shared_ptr<Hash> >::iterator j = hashes_diff.begin(); j != hashes_diff.end(); ++j)
    { (*j) = hash_orig; }
    unsigned int diff_node_nr = rand() % x;
    hashes_diff[diff_node_nr] = hash_diff;

    // getting a map of the changed hashes
    // getting tree depth 
    int depth = 0;
    int j = 1;
    while ( (1 + ((x - 1) / j)) > 1)
    {
      j = j << 1;
      depth += 1;
    }

    ht_diff->makeHashTree(hashes_diff);
    hashes_temp = *(ht_diff->getHashes());
    elp = *(ht_diff->getElementsPerLevel());
    // compare sizes
    if (hashes_orig.size() != hashes_temp.size())
    {
      return_value = false;
      std::cout << "Comparison: Trees have differing sizes." << std::endl;
      break;
    }
    // compare values
    list_of_culprits.clear();
    for (unsigned int j = 0; j < hashes_orig.size(); ++j)
    {
      if (j == diff_node_nr)
      {
        if (hashes_orig[j]->getString() == hashes_temp[j]->getString())
        {
          return_value = false;
          std::cout << "Comparison: faulty hash equaled original hash." << std::endl;
          break; break;
        } else { list_of_culprits.push_back(j); }
      } else if (j < x) {
        if (hashes_orig[j]->getString() != hashes_temp[j]->getString()) {
          return_value = false;
          std::cout << "Comparison: same hashes did not equal." << std::endl;
          break; break;
        }
      } else if (hashes_orig[j]->getString() != hashes_temp[j]->getString()) {
        list_of_culprits.push_back(j);
      }
    }
    if (elp.size() < list_of_culprits.size())
    {
      return_value = false;
      std::cout << "Comparison: There were more differing Hashes than supposed to. " << std::endl;
      std::cout << "elp.size()" << elp.size() << std::endl;
      std::cout << "list_of_culprits.size()" << list_of_culprits.size() << std::endl;
      break;
    } else {
      int node_count = 0;
      for (unsigned int j = 0; j < elp.size(); ++j)
      {
        node_count += elp[j];
        if (list_of_culprits[j] > node_count)
        {
          return_value = false;
          std::cout << "Comparison: There are at least two Hashes on the same Tree-Depth. " << std::endl;
          std::cout << "The Hash-Node is " << list_of_culprits[j] << std::endl;
          break; break;
        }
      }
    }
  }

  delete ht_orig;
  delete ht_diff;

  return return_value;
}