/**
 * \file      hash.hpp
 * \brief     Provides a wrapper class for generating file hashes. 
 *
 *  dkjandjkawndjkawndkjawndkajn
 * 
 * \author    Alexander Herr
 * \date      2016
 * \copyright GNU Public License v3 or higher. 
 */

#ifndef INCLUDE_HASH_HPP_
#define INCLUDE_HASH_HPP_

#include <string>
#include <sstream>
#include <iomanip>
#include <memory>
#include <cstring>

#include <iostream>

#define F_GENERIC_HASH_LEN 32U

/**
 * \brief Wrapper class for hashed strings enabling sorting etc.
 *
 * A wrapper class for the used hashing algorithms so hashed strings or other
 * hashable arrays can be used and stored as objects. Additionally it provides
 * operators for "hash comparison" so hashes can be sorted, which makes 
 * hash-trees possible. 
 *
 * \todo Maybe store byte arrays instead of std::strings?
 * \todo Make other hashable arrays possible for hash generation
 */
class Hash {
 public:
  Hash();
  explicit Hash(const unsigned char hash_bytes[F_GENERIC_HASH_LEN]);
  explicit Hash(const std::string& string);
  ~Hash();

  void makeHash(const std::string& string);
  void initializeHash(const std::string& hash_string);
  void initializeHash(const unsigned char* hash_bytes);

  const std::string getString() const;
  const unsigned char* getBytes() const;

 private:
  unsigned char* hash_;

  friend bool operator<  (const Hash&, const Hash&);
  friend bool operator>  (const Hash&, const Hash&);
  friend bool operator<= (const Hash&, const Hash&);
  friend bool operator>= (const Hash&, const Hash&);
  friend bool operator== (const Hash&, const Hash&);
  friend bool operator!= (const Hash&, const Hash&);
};

inline bool operator<  (const Hash& lhs, const Hash& rhs) {
  return (memcmp(lhs.hash_, rhs.hash_, F_GENERIC_HASH_LEN) < 0)? true : false;
}
inline bool operator>  (const Hash& lhs, const Hash& rhs)
  { return rhs < lhs; }
inline bool operator<= (const Hash& lhs, const Hash& rhs)
  { return !(lhs > rhs); }
inline bool operator>= (const Hash& lhs, const Hash& rhs)
  { return !(lhs < rhs); }
inline bool operator== (const Hash& lhs, const Hash& rhs) {
  return (memcmp(lhs.hash_, rhs.hash_, F_GENERIC_HASH_LEN) == 0) ? true : false;
}
inline bool operator!= (const Hash& lhs, const Hash& rhs)
  { return !(lhs == rhs); }

struct hashPointerLessThanFunctor {
  bool operator()(const Hash* lhs, const Hash* rhs) const
    { return (*lhs < *rhs); }
};
struct hashPointerEqualsFunctor {
  bool operator()(const Hash* lhs, const Hash* rhs) const
    { return ((*lhs) == (*rhs)); }
};
struct hashSharedPointerLessThanFunctor {
  bool operator()
    (const std::shared_ptr<Hash> lhs,
     const std::shared_ptr<Hash> rhs) const
      { return (*lhs < *rhs); }
};
struct hashSharedPointerEqualsFunctor {
  bool operator()
    (const std::shared_ptr<Hash> lhs,
     const std::shared_ptr<Hash> rhs) const
      { return ((*lhs) == (*rhs)); }
};
struct hashAsKeyForContainerFunctor {
    size_t operator()(Hash* hash) const {
        size_t small_hash;
        std::memcpy(&small_hash, hash->getBytes(), sizeof(size_t));
        return small_hash;
    }
};

#endif  // INCLUDE_HASH_HPP_
