#include <boost/test/unit_test.hpp>
#include "hash.hpp"

BOOST_AUTO_TEST_CASE(hash_structors)
{
  Hash hash;
  BOOST_CHECK_EQUAL("", hash.getHash());
}
BOOST_AUTO_TEST_CASE(hash_makeHash)
{
  Hash hash = Hash("test");
  // Value taken from: http://asecuritysite.com/encryption/tiger
  std::string precomputed_hash = "7AB383FC29D81F8D0D68E87C69BAE5F1F18266D730C48B1D";
  std::transform(precomputed_hash.begin(), precomputed_hash.end(), precomputed_hash.begin(), ::tolower);
  BOOST_CHECK_EQUAL(precomputed_hash, hash.getHash());
}
BOOST_AUTO_TEST_CASE(hash_compare)
{
  Hash hash1 = Hash("test1"); // = C099BBD00FAF33027AB55BFB4C3A67F19ECD8EB950078ED2
  Hash hash2 = Hash("test2"); // = 3B1B47A309A5F1BE449E3719E5160C8572C4425685830D2B
                              // therefore hash2 < hash1
  BOOST_CHECK(!(hash1 == hash2));
  BOOST_CHECK(  hash1 != hash2);
  BOOST_CHECK(!(hash1 <  hash2));
  BOOST_CHECK(!(hash1 <= hash2));
  BOOST_CHECK(  hash1 >  hash2);
  BOOST_CHECK(  hash1 >= hash2);
}