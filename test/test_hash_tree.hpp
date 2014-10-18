#include "hash_tree.hpp"
#include "test_hash_tree_helper.hpp"

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
    // to make it at all feasible, the maximium number
    // of values (not nodes) in the tree is limited to 999
    int random = rand() % 999;
    BOOST_CHECK(hash_tree_compare_helper(random));
  }
}
BOOST_AUTO_TEST_CASE(hash_tree_compare_diff)
{
  Hash* hash_orig = new Hash("test");
  Hash* hash_diff = new Hash("test2");
  std::vector<Hash*> hashes_orig(8);
  std::vector<Hash*> hashes_diff(8);
  std::vector<Hash*> changed_hashes;
  HashTree* ht_orig = new HashTree();
  HashTree* ht_diff = new HashTree();

  for (int i = 0; i < 8; ++i) // check all 8 permutations
  {
    hashes_orig.clear();
    hashes_diff.clear();
    changed_hashes.clear();
    for (int j = 0; j < 8; ++j) // fill vectors with permutation
    {
      hashes_orig.push_back(hash_orig);
      if (i==j) { hashes_diff.push_back(hash_diff); }
      else { hashes_diff.push_back(hash_orig); }
    }
    ht_orig->makeHashTree(hashes_orig);
    ht_diff->makeHashTree(hashes_diff);

    BOOST_CHECK(ht_diff->getChangedHashes(changed_hashes, *ht_orig));
    BOOST_CHECK_EQUAL(1, changed_hashes.size());
    BOOST_CHECK(changed_hashes[0] == hashes_diff[i]);
  }
}