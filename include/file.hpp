/**
 * \file      file.hpp
 * \brief     Class for File-IO. 
 *
 *  A class to construct C++ streams from and to, as well as 
 *  writing metadata and data to disk using OS features. 
 *  Needs the path and Hash of a Box the file belongs to, 
 *  constructs the File's path from the box_path and a relative
 *  path supplied. 
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
#include "constants.hpp"
#include "hash.hpp"

/**
 * \brief Class for File-IO. 
 *
 *  A class to construct C++ streams from and to, as well as 
 *  writing metadata and data to disk using OS features. 
 *  Needs the path and Hash of a Box the file belongs to, 
 *  constructs the File's path from the box_path and a relative
 *  path supplied. 
 */
class File {
 public:
//    File() {}
    File(const std::string& box_path,
         Hash* box_hash);
    File(const std::string& box_path,
         Hash* box_hash,
         const std::string& path,
         const boost::filesystem::file_type type,
         const bool create);
    File(const std::string& box_path,
         Hash* box_hash,
         const std::string& path,
         const boost::filesystem::perms mode,
         const boost::filesystem::file_type type,
         const uint32_t mtime,
         const bool create);
    File(const std::string& box_path,
         Hash* box_hash,
         const std::string& path)
      : File(box_path, box_hash, path,
             boost::filesystem::regular_file, false) {}
    File(const std::string& box_path,
         Hash* box_hash,
         const std::string& path,
         const File& file,
         const bool create);
    File(const std::string& box_path,
         Hash* box_hash,
         const std::string& path,
         const File& file)
      : File(box_path, box_hash, path, file, false) {}
    // Since the copy-constructor of std::fstream is explicitly deleted,
    // this Class' copy-constructor should also be explicitly deleted
    File(const File& f) = delete;
    ~File();

    const std::string getPath() const;
    boost::filesystem::perms getMode() const;
    uint32_t getMtime() const;
    uint64_t getSize() const;
    boost::filesystem::file_type getType() const;

    void setMode(boost::filesystem::perms mode);
    void setMtime(uint32_t mtime);
    void storeMetadata() const;
    void resize(uint64_t size);

    void openFile();
    void closeFile();

    uint64_t readFileData(char* data,
                         const uint64_t size,
                         uint64_t offset,
                         bool* more);
    uint64_t readFileData(char* data, uint64_t offset);
    uint64_t readFileData(char* data, uint64_t offset, bool* more);

    void storeFileData(const char* data,
                       const uint64_t size,
                       uint64_t offset);

 private:
    std::string                              box_path_;
    Hash*                                    box_hash_;
    boost::filesystem::path                  bpath_;
    std::array<char, SB_MAXIMUM_PATH_LENGTH> path_;
    boost::filesystem::perms                 mode_;
    uint32_t                                 mtime_;
    boost::filesystem::file_type             type_;
    uint64_t                                 size_;
    std::fstream                             fstream_;

    void checkArguments(const std::string& path,
                        const boost::filesystem::file_type type,
                        const bool create) const;
    void checkArguments(const std::string& path,
                        const bool create) const;

    const std::string constructPath(const std::string box_path,
                                    const std::string path) const;

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
