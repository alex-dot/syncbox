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
#include <boost/filesystem/fstream.hpp>
//#include "constants.hpp"

#define SB_MAXIMUM_PATH_LENGTH 128
#define SB_MAXIMUM_FILE_PACKAGE_SIZE 4096

/**
 * \brief Small class for condensing file information.
 *
 * Small class for condensing file information.
 *
 * \todo handling directories
 * \todo handle offsete at write
 */
class File {
 public:
    File();
    File(const std::string& path, const bool create);
    File(const std::string& path,
         const bool create,
         const boost::filesystem::perms mode,
         const int32_t mtime);
    explicit File(const std::string& path) : File(path, false) {}
    File(const std::string& path, const File& file, const bool create);
    File(const std::string& path, const File& file) : File(path, file, false) {}
    ~File();

    const std::string getPath() const;
    boost::filesystem::perms getMode() const;
    int32_t getMtime() const;
    int64_t getSize() const;

    void setMode(boost::filesystem::perms mode);
    void setMtime(int32_t mtime);
    void storeMetadata() const;
    void resize(int64_t size);

    int64_t readFileData(char* data,
                         const int64_t size,
                         int64_t offset,
                         bool* more) const;
    int64_t readFileData(char* data, int64_t offset) const;
    int64_t readFileData(char* data, int64_t offset, bool* more) const;
    void storeFileData(const char* data, const int64_t size) const;

 private:
    boost::filesystem::path                  bpath_;
    std::array<char, SB_MAXIMUM_PATH_LENGTH> path_;
    boost::filesystem::perms                 mode_;
    int32_t                                  mtime_;
    boost::filesystem::file_type             type_;
    int64_t                                  size_;

    void checkArguments(const std::string& path, const bool create) const;

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
