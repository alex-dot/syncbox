/*
 *
 *
 */

#ifndef SB_HASH_HPP
#define SB_HASH_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include "cryptopp/tiger.h"
typedef char unsigned byte;

// convenience struct which allows for easy transformation
// between unsigned chars (bytes) and (signed) chars. 
// original code courtesy of http://stackoverflow.com/a/673389/876584
struct HexCharStruct
{
  unsigned char c;
  HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
  return (o << std::setw(2) << std::setfill('0') << std::hex << (int)hs.c << std::dec);
}
inline HexCharStruct hex(unsigned char _c)
{
  return HexCharStruct(_c);
}


class Hash
{
  public: 
    Hash();
    Hash(const std::string& string);
    ~Hash();

    void makeHash(const std::string& string);
    const std::string& getHash() const;

  private:
    std::string hash_;
};

inline bool operator<  (const Hash& lhs, const Hash& rhs)
{
  return (lhs.getHash() < rhs.getHash()) ? true : false;
}
inline bool operator>  (const Hash& lhs, const Hash& rhs) { return rhs < lhs; }
inline bool operator<= (const Hash& lhs, const Hash& rhs) { return !(lhs > rhs); }
inline bool operator>= (const Hash& lhs, const Hash& rhs) { return !(lhs < rhs); }
inline bool operator== (const Hash& lhs, const Hash& rhs)
{
  return (lhs.getHash() == rhs.getHash()) ? true : false;
}
inline bool operator!= (const Hash& lhs, const Hash& rhs) { return !(lhs == rhs); }

struct hashPointerLessThanFunctor
{
  bool operator()(const Hash* lhs, const Hash* rhs) { return (*lhs < *rhs); }
};
struct hashPointerEqualsFunctor
{
  bool operator()(const Hash* lhs, const Hash* rhs) { return ((*lhs) == (*rhs)); }
};
struct hashSharedPointerLessThanFunctor
{
  bool operator()(const std::shared_ptr<Hash> lhs, const std::shared_ptr<Hash> rhs) { return (*lhs < *rhs); }
};
struct hashSharedPointerEqualsFunctor
{
  bool operator()(const std::shared_ptr<Hash> lhs, const std::shared_ptr<Hash> rhs) { return ((*lhs) == (*rhs)); }
};

#endif