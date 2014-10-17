#include "hash_tree.hpp"

bool hash_tree_compare_helper(unsigned int x)
{
  std::cout << "Testing Tree with " << x << " values." << std::endl;

  bool return_value = true;
  Hash* hash_orig = new Hash("test");
  Hash* hash_diff = new Hash("test2");
  std::vector<Hash*> hashes_orig(x);
  std::vector<Hash*> hashes_diff(x);
  std::vector<Hash*> hashes_temp;
  std::vector<int> elp;
  HashTree* ht_orig = new HashTree();
  HashTree* ht_diff = new HashTree();
  
  for (std::vector<Hash*>::iterator i = hashes_orig.begin(); i != hashes_orig.end(); ++i)
  { (*i) = hash_orig; }
  ht_orig->makeHashTree(hashes_orig);
  hashes_orig.clear();
  hashes_orig = *(ht_orig->getHashes());
  int rounds = (x>100) ? 100 : x;
  std::vector<int> list_of_culprits(10000);   // a list of differing hashes
  for (int i = 0; i < rounds; ++i)
  {
    for (std::vector<Hash*>::iterator j = hashes_diff.begin(); j != hashes_diff.end(); ++j)
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
        if (hashes_orig[j]->getHash() == hashes_temp[j]->getHash())
        {
          return_value = false;
          std::cout << "Comparison: faulty hash equaled original hash." << std::endl;
          break; break;
        } else { list_of_culprits.push_back(j); }
      } else if (j < x) {
        if (hashes_orig[j]->getHash() != hashes_temp[j]->getHash()) {
          return_value = false;
          std::cout << "Comparison: same hashes did not equal." << std::endl;
          break; break;
        }
      } else if (hashes_orig[j]->getHash() != hashes_temp[j]->getHash()) {
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

  delete hash_orig;
  delete hash_diff;
  delete ht_orig;
  delete ht_diff;

  return return_value;
}

BOOST_AUTO_TEST_CASE(hash_tree_size_compare)
{
  Hash* hash = new Hash("test");
  std::vector<Hash*> hashes;
  // 1 node
  hashes.push_back(hash);
  HashTree* ht = new HashTree(hashes);
  ht->makeHashTreeFromSelf();
  BOOST_CHECK_EQUAL(1, ht->getHashes()->size());
  // 2 nodes
  hashes.push_back(hash);
  ht->makeHashTree(hashes);
  BOOST_CHECK_EQUAL(3, ht->getHashes()->size());
  // 3 nodes
  hashes.push_back(hash);
  ht->makeHashTree(hashes);
  BOOST_CHECK_EQUAL(6, ht->getHashes()->size());
  // 4 nodes
  hashes.push_back(hash);
  ht->makeHashTree(hashes);
  BOOST_CHECK_EQUAL(7, ht->getHashes()->size());
  // 5 nodes
  hashes.push_back(hash);
  ht->makeHashTree(hashes);
  BOOST_CHECK_EQUAL(11, ht->getHashes()->size());
  // 7 nodes
  hashes.push_back(hash);
  hashes.push_back(hash);
  ht->makeHashTree(hashes);
  BOOST_CHECK_EQUAL(14, ht->getHashes()->size());
  // 9 nodes
  hashes.push_back(hash);
  hashes.push_back(hash);
  ht->makeHashTree(hashes);
  BOOST_CHECK_EQUAL(20, ht->getHashes()->size());
  // 16 nodes
  for (int i = 0; i < 7; ++i)
  {
    hashes.push_back(hash);
  }
  ht->makeHashTree(hashes);
  BOOST_CHECK_EQUAL(31, ht->getHashes()->size());
  // 17 nodes
  hashes.push_back(hash);
  ht->makeHashTree(hashes);
  BOOST_CHECK_EQUAL(37, ht->getHashes()->size());

  delete hash;
  delete ht;
}
BOOST_AUTO_TEST_CASE(hash_tree_size_compare_random)
{
  Hash* hash = new Hash("test");
  std::vector<Hash*> hashes;
  HashTree* ht = new HashTree();
  for (int i = 0; i < 5; ++i)
  {
    int size = 0;
    int random = rand() % 65500;
    hashes.clear();
    hashes.resize(random,hash);
    ht->makeHashTree(hashes);

    int j = 1;
    int depth = 0;
    while ( (1 + ((random - 1) / j)) > 1)
    {
      j = j << 1;
      depth += 1;
    }
    size += random;
    int divident = random;
    for (j = depth; j > 0; --j)
    {
      divident = (divident % 2 == 0) ? (divident / 2) : ((divident+1) / 2);
      size += divident;
    }
    BOOST_CHECK_EQUAL(size, ht->getHashes()->size());
  }

  delete hash;
  delete ht;
}
BOOST_AUTO_TEST_CASE(hash_tree_compare)
{
  for (int i = 0; i < 10; ++i)
  {
    // this check is intentionally slow
    // to make it at all feasible (<60s), the maximium number
    // of values (not nodes) in the tree is limited to 999
    int random = rand() % 999;
    BOOST_CHECK(hash_tree_compare_helper(random));
  }
}