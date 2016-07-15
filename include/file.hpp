/**
 * \file      file.hpp
 * \brief     Small class for condensing file information.
 *
 *  Small class for condensing file information.
 * 
 * \author    Alexander Herr
 * \date      2016
 * \copyright GNU Public License v3 or higher. 
 */

#ifndef INCLUDE_FILE_HPP_
#define INCLUDE_FILE_HPP_

#include <string>
#include <array>
#include <ios>
#include <boost/filesystem.hpp>
//#include "constants.hpp"

#define SB_MAXIMUM_PATH_LENGTH 128

/**
 * \brief Small class for condensing file information.
 *
 * Small class for condensing file information.
 *
 */
class File {
 public:
    File();
    explicit File(const std::string& path);
    ~File();

    const std::string getPath() const;

 private:
    std::array<char, SB_MAXIMUM_PATH_LENGTH> path_;
    boost::filesystem::perms                 mode_;
    int32_t                                  mtime_;
    boost::filesystem::file_type             type_;

    friend std::ostream& operator<<(std::ostream& ostream, const File& f);
    friend std::istream& operator>>(std::istream& istream, File& f);
};

inline bool operator<  (const File& lhs, const File& rhs) {
    return (lhs.getPath() == rhs.getPath()) ? true : false;
}
inline bool operator>  (const File& lhs, const File& rhs)
    { return rhs < lhs; }
inline bool operator<= (const File& lhs, const File& rhs)
    { return !(lhs > rhs); }
inline bool operator>= (const File& lhs, const File& rhs)
    { return !(lhs < rhs); }
inline bool operator== (const File& lhs, const File& rhs) {
    return (lhs.getPath() == rhs.getPath()) ? true : false;
}
inline bool operator!= (const File& lhs, const File& rhs)
    { return !(lhs == rhs); }
std::ostream& operator<<(std::ostream& ostream, const File& f);
std::istream& operator>>(std::istream& istream, File& f);


#endif  // INCLUDE_FILE_HPP_
