#include "box.hpp"

BOOST_AUTO_TEST_CASE(box_test)
{
  boost::filesystem::path p = boost::filesystem::current_path().string() + "/../../test/testdir";
  Box* box = new Box(p);
  HashTree* ht = box->getHashTree();
  std::vector<Hash*> hashes = *(ht->getHashes());
  if (hashes.size() == 3)
  {
    std::cout << "Note that these tests very likely fail, because the Dir Object "
              << "will take the timestamp into account. " << std::endl;
    // These values are taken from: http://asecuritysite.com/encryption/tiger
    std::string hash1 = "6825BF644BCF4CF78D312C6A1FF83F9B1CC655F9CBFD2CF5";
    std::transform(hash1.begin(), hash1.end(), hash1.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash1, hashes[0]->getHash());
    std::string hash2 = "6825BF644BCF4CF78D312C6A1FF83F9B1CC655F9CBFD2CF5";
    std::transform(hash2.begin(), hash2.end(), hash2.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash2, hashes[1]->getHash());
    std::string hash3 = "03FE5169A2A07DE4E4B8E8C2ACBA094BF55AB0E4CDAF4208";
    std::transform(hash3.begin(), hash3.end(), hash3.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash3, hashes[2]->getHash());
  } else {
    std::cout << "There were more than 2 Directories in the testdir. "
              << "Did someone change files? " << std::endl;
    BOOST_CHECK(false);
  }
}
BOOST_AUTO_TEST_CASE(box_compare)
{
  boost::filesystem::path p1 = boost::filesystem::current_path().string() + "/../../test/testdir/testdir";
  boost::filesystem::path p2 = boost::filesystem::current_path().string() + "/../../test/testdir/testdir2";
  std::vector<Hash*> hashes;
  Box box1(p1);
  // compare same/unchanged dir
  Box box2(p1);
  box1.getChangedDirHashes(hashes, box2);
  BOOST_CHECK_EQUAL(0,hashes.size());
  // compare different dirs (one item extra)
  // this makes testdir and testdir2 completely different
  // therefore we expect two hashes, one for testdir and one for testdir2
  box2 = Box(p2);
  box1.getChangedDirHashes(hashes, box2);
  BOOST_CHECK_EQUAL(2,hashes.size());
}