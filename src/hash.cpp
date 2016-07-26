/**
 * \file      hash.cpp
 * \brief     Provides a wrapper class for generating file hashes. 
 * \author    Alexander Herr
 * \date      2014-2016
 * \copyright GNU Public License v3 or higher. 
 */

#include "hash.hpp"

#include <sodium.h>
#include <string>
#include <sstream>
#include <iomanip>

#include "constants.hpp"

Hash::Hash() : hash_(new unsigned char[SB_GENERIC_HASH_LEN]) {}
Hash::Hash(const std::string& string) : hash_() {
    makeHash(string);
}
Hash::~Hash() {}

/**
 * \fn Hash::makeHash
 *
 * Generates a cryptographic hash out of the supplied string and stores it in 
 * the hash_ member variable. Uses libsodiums generichash (Blake2) for hashes. 
 * 
 * \param string
 */
void Hash::makeHash(const std::string& string) {
    crypto_generichash(
        hash_, SB_GENERIC_HASH_LEN,
        (unsigned char*)string.c_str(),
        string.length(),
        NULL, 0);
}
void Hash::initializeHash(const std::string& hash_string) {
    hash_ = (unsigned char*)hash_string.c_str();
}
const std::string Hash::getString() const {
  return std::string(reinterpret_cast<char*>(hash_));
}
