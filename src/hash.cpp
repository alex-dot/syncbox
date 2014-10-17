/*
 * Hash
 *
 */

#include "hash.hpp"

#include <string>
#include <sstream>
#include <iomanip>
#include "cryptopp/tiger.h"

Hash::Hash() : hash_(){}
Hash::Hash(const std::string& string) : hash_()
{
  makeHash(string);
}
Hash::~Hash(){}

void Hash::makeHash(const std::string& string)
{
  byte const* string_input = (unsigned char*)string.c_str();
  unsigned int string_input_length = string.length();

  byte digest[CryptoPP::Tiger::DIGESTSIZE]; // = digest[24]

  CryptoPP::Tiger hash;
  hash.Update(string_input,string_input_length);
  hash.Final(digest);

  std::ostringstream stream;
  for ( int i = 0; i < CryptoPP::Tiger::DIGESTSIZE; ++i )
  {
    stream << hex(digest[i]);
  }
  hash_ = stream.str();
}
const std::string& Hash::getHash() const
{
  return hash_;
}