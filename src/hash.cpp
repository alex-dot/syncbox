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

#include <iostream>

Hash::Hash() :
        hash_(new unsigned char[F_GENERIC_HASH_LEN]) {}
Hash::Hash(const unsigned char hash_bytes[F_GENERIC_HASH_LEN]) :
        hash_(new unsigned char[F_GENERIC_HASH_LEN]) {
    std::memcpy(hash_, hash_bytes, F_GENERIC_HASH_LEN);
}
Hash::Hash(const std::string& string) :
        hash_(new unsigned char[F_GENERIC_HASH_LEN]) {
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
    unsigned char new_hash[F_GENERIC_HASH_LEN];
    crypto_generichash(
        new_hash, F_GENERIC_HASH_LEN,
        (unsigned char*)string.c_str(),
        string.length(),
        NULL, 0);
    std::memcpy(hash_, new_hash, F_GENERIC_HASH_LEN);
}
void Hash::initializeHash(const std::string& hash_string) {
    hash_ = (unsigned char*)hash_string.c_str();
}
void Hash::initializeHash(const unsigned char* hash_bytes) {
    std::memcpy(hash_, hash_bytes, F_GENERIC_HASH_LEN);
}
const std::string Hash::getString() const {
  return std::string(reinterpret_cast<char*>(hash_));
}
const unsigned char* Hash::getBytes() const {
  return hash_;
}
