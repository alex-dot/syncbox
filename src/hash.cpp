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

Hash::Hash() : hash_() {}
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
    unsigned char* new_hash = new unsigned char[256];
    crypto_generichash(
        new_hash, 256,
        (unsigned char*)string.c_str(),
        string.length(),
        NULL, 0);
    hash_ = std::string(reinterpret_cast<char*>(new_hash));
}
void Hash::initializeHash(const std::string& hash_string) {
    hash_ = hash_string;
}
const std::string& Hash::getString() const {
  return hash_;
}
