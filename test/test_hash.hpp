#include "hash.hpp"

BOOST_AUTO_TEST_CASE(hash_structors)
{
  Hash hash;
  BOOST_CHECK_EQUAL("", hash.getHash());
}
BOOST_AUTO_TEST_CASE(hash_makeHash)
{
  Hash hash = Hash("test");
  BOOST_TEST_MESSAGE(hash.getHash());
  // precomputed hash value for "test"
  BOOST_CHECK_EQUAL("7ab383fc29d81f8d0d68e87c69bae5f1f18266d730c48b1d", hash.getHash());
}