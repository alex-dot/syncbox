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
#include "cryptopp/tiger.h"

typedef char unsigned byte;  // this should probably appear elsewhere?


/**
 * \brief Convenience struct for transformation between unsigned and signed chars.
 *
 * Conveniently allows easy transformation between unsigned chars (bytes)
 * and (signed) chars. 
 * Original code from http://stackoverflow.com/a/673389/876584
 * \author ???
 */
struct HexCharStruct {
    unsigned char c;
    explicit HexCharStruct(unsigned char _c) : c(_c) { }
};
inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs) {
    return (o << std::setw(2) << std::setfill('0') << std::hex
              << static_cast<int>(hs.c) << std::dec);
}
inline HexCharStruct hex(unsigned char _c) {
    return HexCharStruct(_c);
}

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
    explicit Hash(const std::string& string);
    ~Hash();

    void makeHash(const std::string& string);
    const std::string& getHash() const;

 private:
    std::string hash_;
};

inline bool operator<  (const Hash& lhs, const Hash& rhs) {
    return (lhs.getHash() < rhs.getHash()) ? true : false;
}
inline bool operator>  (const Hash& lhs, const Hash& rhs)
    { return rhs < lhs; }
inline bool operator<= (const Hash& lhs, const Hash& rhs)
    { return !(lhs > rhs); }
inline bool operator>= (const Hash& lhs, const Hash& rhs)
    { return !(lhs < rhs); }
inline bool operator== (const Hash& lhs, const Hash& rhs) {
    return (lhs.getHash() == rhs.getHash()) ? true : false;
}
inline bool operator!= (const Hash& lhs, const Hash& rhs)
    { return !(lhs == rhs); }

struct hashPointerLessThanFunctor {
    bool operator()(const Hash* lhs, const Hash* rhs)
        { return (*lhs < *rhs); }
};
struct hashPointerEqualsFunctor {
    bool operator()(const Hash* lhs, const Hash* rhs)
        { return ((*lhs) == (*rhs)); }
};
struct hashSharedPointerLessThanFunctor {
    bool operator()
        (const std::shared_ptr<Hash> lhs,
         const std::shared_ptr<Hash> rhs)
            { return (*lhs < *rhs); }
};
struct hashSharedPointerEqualsFunctor {
    bool operator()
        (const std::shared_ptr<Hash> lhs,
         const std::shared_ptr<Hash> rhs)
            { return ((*lhs) == (*rhs)); }
};

#endif  // INCLUDE_HASH_HPP_
