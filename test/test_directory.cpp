#include <boost/test/unit_test.hpp>
#include "directory.hpp"
#include <string>

BOOST_AUTO_TEST_CASE(directory_manual)
{
  Directory dir;
  boost::filesystem::path p = boost::filesystem::current_path().string() + "/../../test/testdir";

  std::vector<boost::filesystem::directory_entry> dirs;
  dir.fillDirectory(p, dirs);
  HashTree* ht = dir.getHashTree();
  std::vector< std::shared_ptr<Hash> > hashes = *(ht->getHashes());
  if (hashes.size() == 6)
  {
    std::cout << "Note that these tests very likely fail, because the Dir Object "
              << "will take the timestamp into account for hash creation. " << std::endl;
    // These values are taken from: http://asecuritysite.com/encryption/tiger
    std::string hash1 = "2FF23AD4B825575D10DD8022943E3B25284D70365A03B25C";
    std::transform(hash1.begin(), hash1.end(), hash1.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash1, hashes[0]->getHash());
    std::string hash2 = "5570F8C658E96EC453582005E21A09656C7181073CCE8C3B";
    std::transform(hash2.begin(), hash2.end(), hash2.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash2, hashes[1]->getHash());
    std::string hash3 = "AC47BA08076B241D1B2E04347279A224AB6DDE594C69A74C";
    std::transform(hash3.begin(), hash3.end(), hash3.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash3, hashes[2]->getHash());
    std::string hash4 = "1FCEBAE57D94C4E7841B5B454542925E8E2EAFE557D86004";
    std::transform(hash4.begin(), hash4.end(), hash4.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash4, hashes[3]->getHash());
    std::string hash5 = "850F4F9A019925CC63CCA7B3B994618857D23E465F1ED4F6";
    std::transform(hash5.begin(), hash5.end(), hash5.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash5, hashes[4]->getHash());
    std::string hash6 = "6825BF644BCF4CF78D312C6A1FF83F9B1CC655F9CBFD2CF5";
    std::transform(hash6.begin(), hash6.end(), hash6.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash6, hashes[5]->getHash());
  } else {
    std::cout << "There were more than 6 hashes in the test dir. "
              << "Did someone change files? " << std::endl;
    BOOST_CHECK(false);
  }
}
BOOST_AUTO_TEST_CASE(directory_auto)
{
  boost::filesystem::path p = boost::filesystem::current_path().string() + "/../../test/testdir";
  Directory dir(p);
  HashTree* ht = dir.getHashTree();
  std::vector< std::shared_ptr<Hash> > hashes = *(ht->getHashes());
  if (hashes.size() == 6)
  {
    std::cout << "Note that these tests very likely fail, because the Dir Object "
              << "will take the timestamp into account for hash creation. " << std::endl;
    // These values are taken from: http://asecuritysite.com/encryption/tiger
    std::string hash1 = "2FF23AD4B825575D10DD8022943E3B25284D70365A03B25C";
    std::transform(hash1.begin(), hash1.end(), hash1.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash1, hashes[0]->getHash());
    std::string hash2 = "5570F8C658E96EC453582005E21A09656C7181073CCE8C3B";
    std::transform(hash2.begin(), hash2.end(), hash2.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash2, hashes[1]->getHash());
    std::string hash3 = "AC47BA08076B241D1B2E04347279A224AB6DDE594C69A74C";
    std::transform(hash3.begin(), hash3.end(), hash3.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash3, hashes[2]->getHash());
    std::string hash4 = "1FCEBAE57D94C4E7841B5B454542925E8E2EAFE557D86004";
    std::transform(hash4.begin(), hash4.end(), hash4.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash4, hashes[3]->getHash());
    std::string hash5 = "850F4F9A019925CC63CCA7B3B994618857D23E465F1ED4F6";
    std::transform(hash5.begin(), hash5.end(), hash5.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash5, hashes[4]->getHash());
    std::string hash6 = "6825BF644BCF4CF78D312C6A1FF83F9B1CC655F9CBFD2CF5";
    std::transform(hash6.begin(), hash6.end(), hash6.begin(), ::tolower);
    BOOST_CHECK_EQUAL(hash6, hashes[5]->getHash());
  } else {
    std::cout << "There were more than 6 hashes in the test dir. "
              << "Did someone change files? " << std::endl;
    BOOST_CHECK(false);
  }
}
BOOST_AUTO_TEST_CASE(directory_compare)
{
  boost::filesystem::path p1 = boost::filesystem::current_path().string() + "/../../test/testdir/testdir";
  boost::filesystem::path p2 = boost::filesystem::current_path().string() + "/../../test/testdir/testdir2";
  std::vector< std::shared_ptr<Hash> > hashes;
  Directory dir1(p1);
  // compare same/unchanged dir
  Directory dir2(p1);
  dir1.getChangedEntryHashes(hashes, dir2);
  BOOST_CHECK_EQUAL(0,hashes.size());
  // compare different dirs (one item extra)
  Directory dir3(p2);
  dir1.getChangedEntryHashes(hashes, dir3);
  BOOST_CHECK_EQUAL(1,hashes.size());
}