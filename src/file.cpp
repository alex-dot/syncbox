/**
 * \file      file.cpp
 * \brief     Small class for condensing file information.
 * \author    Alexander Herr
 * \date      2014-2016
 * \copyright GNU Public License v3 or higher. 
 */

#include "file.hpp"

#include <endian.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <iomanip>

File::File() :
            bpath_(),
            path_(),
            mode_(),
            mtime_(),
            type_(),
            size_(),
            fstream_() {}
File::File(const std::string& path,
           const boost::filesystem::file_type type,
           const bool create) :
            bpath_(),
            path_(),
            mode_(),
            mtime_(),
            type_(type),
            size_(),
            fstream_() {
  bpath_ = boost::filesystem::path(path);
  checkArguments(path, type, create);

  std::copy(path.begin(), path.end(), path_.begin());

  if (boost::filesystem::exists(bpath_) || !create) {
    mtime_ = boost::filesystem::last_write_time(bpath_);
    boost::filesystem::file_status stat = boost::filesystem::status(bpath_);
    mode_ = stat.permissions();
    if (boost::filesystem::is_regular_file(bpath_))
      size_ = boost::filesystem::file_size(bpath_);
  }
}
File::File(const std::string& path,
           const bool create,
           const boost::filesystem::perms mode,
           const boost::filesystem::file_type type,
           const uint32_t mtime) :
            bpath_(),
            path_(),
            mode_(mode),
            mtime_(mtime),
            type_(type),
            size_(),
            fstream_() {
  bpath_ = boost::filesystem::path(path);
  checkArguments(path, type, create);

  std::copy(path.begin(), path.end(), path_.begin());

  if (create)
    storeMetadata();
}
File::File(const std::string& path, const File& file, const bool create) :
            bpath_(),
            path_(),
            mode_(),
            mtime_(),
            type_(),
            size_(),
            fstream_() {
  bpath_ = boost::filesystem::path(path);
  checkArguments(path, file.getType(), create);

  std::copy(path.begin(), path.end(), path_.begin());
  type_ = file.getType();
  mode_ = file.getMode();
  mtime_ = file.getMtime();

  if (create) {
    storeMetadata();
    resize(file.getSize());
  }
}
File::~File() {
  if (fstream_.is_open())
    fstream_.close();
}

void File::checkArguments(const std::string& path,
                          const boost::filesystem::file_type type,
                          const bool create) const {
  // check if the path is not too long
  if (path.length() > SB_MAXIMUM_PATH_LENGTH) {
    std::stringstream error_msg;
    error_msg << "File path too long. "
      << "Currently supports paths up to " << SB_MAXIMUM_PATH_LENGTH
      << " characters. The file " << path << " was "<< path.length()
      << " characters long. ";
    throw std::range_error(error_msg.str());
  }

  // check if the file exists
  boost::system::error_code ec;
  if (!boost::filesystem::exists(bpath_, ec)) {
    if (create) {
      boost::filesystem::path bpath_parent;
      if (path.back() == '/')
        bpath_parent = bpath_.parent_path().parent_path();
      else
        bpath_parent = bpath_.parent_path();

      if (boost::filesystem::exists(bpath_parent, ec)) {
        switch (type) {
          case (boost::filesystem::regular_file) : {
            boost::filesystem::ofstream ofs(bpath_);
            ofs.close();
            break;
          }

          case (boost::filesystem::directory_file) : {
            boost::filesystem::create_directory(bpath_);
            break;
          }

          default : {
            throw boost::filesystem::filesystem_error("", bpath_, ec);
          }
        }
      } else {
        throw boost::filesystem::filesystem_error(
          "Parent directory does not exist", bpath_, ec);
      }
    } else {
      throw boost::filesystem::filesystem_error("", bpath_, ec);
    }
  }
}
void File::checkArguments(const std::string& path,
                          const bool create) const {
  checkArguments(path, boost::filesystem::regular_file, create);
}

const std::string File::getPath() const {
  std::string path(path_.begin(), path_.end());
  return path;
}
boost::filesystem::perms File::getMode() const {
  return mode_;
}
uint32_t File::getMtime() const {
  return mtime_;
}
uint64_t File::getSize() const {
  return size_;
}
boost::filesystem::file_type File::getType() const {
  return type_;
}

void File::setMode(boost::filesystem::perms mode) {
  mode_ = mode;
}
void File::setMtime(uint32_t mtime) {
  mtime_ = mtime;
}

void File::storeMetadata() const {
  boost::filesystem::permissions(bpath_, mode_);
  boost::filesystem::last_write_time(bpath_, static_cast<time_t>(mtime_));
}

void File::resize(uint64_t size) {
  size_ = size;
  boost::system::error_code ec;
  boost::filesystem::resize_file(bpath_, size_, ec);
  if (ec != 0)
    throw boost::filesystem::filesystem_error("", bpath_, ec);
}

void File::openFile() {
  fstream_.open(bpath_.string(), std::ios_base::in | std::ios_base::out);
}
void File::closeFile() {
  fstream_.close();
}

uint64_t File::readFileData(char* data,
                           const uint64_t size,
                           uint64_t offset,
                           bool* more) {
  if (!fstream_.is_open())
    openFile();

  uint64_t data_size;
  if (size_ <= SB_MAXIMUM_FILE_PACKAGE_SIZE && size_ <= size) {
    fstream_.seekg(offset, std::ios_base::beg);
    fstream_.read(data, size_);
    data_size = size_;
  } else if (size < SB_MAXIMUM_FILE_PACKAGE_SIZE) {
    fstream_.seekg(offset, std::ios_base::beg);
    fstream_.read(data, size);
    data_size = size;
  } else {
    fstream_.seekg(offset, std::ios_base::beg);
    fstream_.read(data, SB_MAXIMUM_FILE_PACKAGE_SIZE);
    data_size = SB_MAXIMUM_FILE_PACKAGE_SIZE;
  }

  if (offset + size < size_) {
    *more = true;
  } else {
    *more = false;
  }

  return data_size;
}
uint64_t File::readFileData(char* data, uint64_t offset) {
  return readFileData(data, SB_MAXIMUM_FILE_PACKAGE_SIZE, offset, new bool());
}
uint64_t File::readFileData(char* data, uint64_t offset, bool* more) {
  return readFileData(data, SB_MAXIMUM_FILE_PACKAGE_SIZE, offset, more);
}

void File::storeFileData(const char* data,
                         const uint64_t size,
                         uint64_t offset) {
  if (!fstream_.is_open())
    openFile();

  if (size_ <= SB_MAXIMUM_FILE_PACKAGE_SIZE && size_ <= size) {
    fstream_.seekp(offset, std::ios_base::beg);
    fstream_.write(data, size_);
  } else if (size < SB_MAXIMUM_FILE_PACKAGE_SIZE) {
    fstream_.seekp(offset, std::ios_base::beg);
    fstream_.write(data, size);
  } else {
    fstream_.seekp(offset, std::ios_base::beg);
    fstream_.write(data, SB_MAXIMUM_FILE_PACKAGE_SIZE);
  }

  boost::filesystem::last_write_time(bpath_, static_cast<time_t>(mtime_));
}

std::ostream& operator<<(std::ostream& ostream, const File& f) {
  std::string path(f.path_.begin(), f.path_.end());
  ostream.write(path.c_str(), SB_MAXIMUM_PATH_LENGTH);

  uint16_t mode = htobe16(f.mode_);
  ostream.write((const char*)&mode, 2);

  ostream.write((const char*)&f.type_, 1);

  uint32_t mtime = htobe32(f.mtime_);
  ostream.write((const char*)&mtime, 4);

  if (f.type_ == boost::filesystem::regular_file) {
    uint64_t size = htobe64(f.size_);
    char* size_char = new char[8];
    std::memcpy(size_char, &size, 8);
    ostream.write(size_char, 8);
  }

  return ostream;
}

std::istream& operator>>(std::istream& istream, File& f) {
  char* path_c = new char[SB_MAXIMUM_PATH_LENGTH];
  istream.read(path_c, SB_MAXIMUM_PATH_LENGTH);
  for (int i = 0; i < SB_MAXIMUM_PATH_LENGTH; ++i) {
    f.path_[i] = path_c[i];
  }
  std::string path(f.path_.begin(), f.path_.end());
  f.bpath_ = boost::filesystem::path(path);

  char* mode_c = new char[2];
  istream.read(mode_c, 2);
  int16_t mode;
  std::memcpy(&mode, mode_c, 2);
  f.mode_ = boost::filesystem::perms(be16toh(mode));

  char* type_c = new char[1];
  istream.read(type_c, 1);
  int8_t type;
  std::memcpy(&type, type_c, 1);
  f.type_ = boost::filesystem::file_type(type);

  char* mtime = new char[4];
  istream.read(mtime, 4);
  f.mtime_ = be32toh((int32_t)*mtime);

  f.checkArguments(path, f.type_, true);

  if (f.type_ == boost::filesystem::regular_file) {
    char* size_c = new char[8];
    istream.read(size_c, 8);
    uint64_t size;
    std::memcpy(&size, size_c, 8);
    f.resize(be64toh(size));
  }
  return istream;
}
