#include <boost/test/unit_test.hpp>
#include "hash_tree.hpp"
#include "test_hash_tree_helper.hpp"
#include <algorithm>

BOOST_AUTO_TEST_CASE(hash_tree_size_compare)
{
  std::shared_ptr<Hash> hash(new Hash("test"));
  std::vector< std::shared_ptr<Hash> > hashes;
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

  delete ht;
}
BOOST_AUTO_TEST_CASE(hash_tree_size_compare_random)
{
  std::shared_ptr<Hash> hash(new Hash("test"));
  std::vector< std::shared_ptr<Hash> > hashes;
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
  std::shared_ptr<Hash> hash_orig(new Hash("test"));  // 7ab383fc29d81f8d0d68e87c69bae5f1f18266d730c48b1d
  std::shared_ptr<Hash> hash_diff(new Hash("test2")); // 3b1b47a309a5f1be449e3719e5160c8572c4425685830d2b
  std::vector< std::shared_ptr<Hash> > hashes_orig(8);
  std::vector< std::shared_ptr<Hash> > hashes_diff(8);
  std::vector< std::shared_ptr<Hash> > changed_hashes;
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
    BOOST_CHECK_EQUAL(changed_hashes[0], hashes_diff[0]);
  }
}
BOOST_AUTO_TEST_CASE(hash_tree_sort)
{
  std::shared_ptr<Hash> hash1(new Hash("test1")); // C099BBD00FAF33027AB55BFB4C3A67F19ECD8EB950078ED2
  std::shared_ptr<Hash> hash2(new Hash("test2")); // 3B1B47A309A5F1BE449E3719E5160C8572C4425685830D2B
  std::shared_ptr<Hash> hash3(new Hash("test3")); // B8B9F8AB7E7B617ABD37E86B89DEE671F6332AF9A4088497
  std::shared_ptr<Hash> hash4(new Hash("test4")); // 7A8CFACA415DFD2ACB4930F4D8EA4D7477D0622B61736CB7
  std::shared_ptr<Hash> hash5(new Hash("test5")); // 550F59F87EFA94C8A9E04D7238064BBE29D221097CEBD9B3
  // therefore the correct order would be:
  // hash2, hash5, hash4, hash3, hash1
  std::vector< std::shared_ptr<Hash> > hashes = {hash1, hash2, hash3, hash4, hash5};
  std::sort (hashes.begin(), hashes.end(), hashSharedPointerLessThanFunctor());
  BOOST_CHECK_EQUAL(hash2->getString(), hashes[0]->getString());
  BOOST_CHECK_EQUAL(hash5->getString(), hashes[1]->getString());
  BOOST_CHECK_EQUAL(hash4->getString(), hashes[2]->getString());
  BOOST_CHECK_EQUAL(hash3->getString(), hashes[3]->getString());
  BOOST_CHECK_EQUAL(hash1->getString(), hashes[4]->getString());
  BOOST_CHECK_EQUAL(hash2->getString(), hashes[0]->getString());
}
BOOST_AUTO_TEST_CASE(hash_tree_top_hash)
{
  std::shared_ptr<Hash> hash(new Hash("test"));
  std::vector< std::shared_ptr<Hash> > hashes = {hash, hash, hash};
  HashTree* ht = new HashTree(hashes);
  ht->makeHashTreeFromSelf();
  // Value taken from: http://asecuritysite.com/encryption/tiger
  std::string hash_string = "2FB13E2BC6157D929F28B44E51F563D9812787CD6BAAB82E";
  std::transform(hash_string.begin(), hash_string.end(), hash_string.begin(), ::tolower);
  BOOST_CHECK_EQUAL(hash_string, ht->getTopHash()->getString());
}