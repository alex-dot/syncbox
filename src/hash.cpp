/**
 * \file      hash.cpp
 * \brief     Provides a wrapper class for generating file hashes. 
 * \author    Alexander Herr
 * \date      2014-2016
 * \copyright GNU Public License v3 or higher. 
 */

#include "hash.hpp"

#include <string>
#include <sstream>
#include <iomanip>
#include "cryptopp/tiger.h"

Hash::Hash() : hash_() {}
Hash::Hash(const std::string& string) : hash_() {
    makeHash(string);
}
Hash::~Hash() {}

/**
 * \fn Hash::makeHash
 *
 * Generates a cryptographic hash out of the supplied string and stores it in 
 * the hash_ member variable. Uses Tiger as the cryptographic hash function. 
 * 
 * \param string
 */
void Hash::makeHash(const std::string& string) {
    byte const* string_input = (unsigned char*)string.c_str();
    unsigned int string_input_length = string.length();

    byte digest[CryptoPP::Tiger::DIGESTSIZE];  // = digest[24]

    CryptoPP::Tiger hash;
    hash.Update(string_input, string_input_length);
    hash.Final(digest);

    std::ostringstream stream;
    for ( int i = 0; i < CryptoPP::Tiger::DIGESTSIZE; ++i ) {
      stream << hex(digest[i]);
    }
    hash_ = stream.str();
}
const std::string& Hash::getHash() const {
  return hash_;
}
