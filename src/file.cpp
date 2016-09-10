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

File::File(const std::string& box_path,
           Hash* box_hash) :
            box_path_(box_path),
            box_hash_(box_hash),
            bpath_(),
            path_(),
            mode_(),
            mtime_(),
            type_(),
            size_(),
            deleted_file_(false),
            fstream_() {}
File::File(const std::string& box_path,
           Hash* box_hash,
           const std::string& path,
           const boost::filesystem::file_type type,
           const bool create) :
            box_path_(box_path),
            box_hash_(box_hash),
            bpath_(),
            path_(),
            mode_(),
            mtime_(),
            type_(type),
            size_(),
            deleted_file_(false),
            fstream_() {
  bpath_ = boost::filesystem::path(constructPath(box_path, path));
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
File::File(const std::string& box_path,
           Hash* box_hash,
           const std::string& path,
           const boost::filesystem::perms mode,
           const boost::filesystem::file_type type,
           const uint32_t mtime,
           const bool create) :
            box_path_(box_path),
            box_hash_(box_hash),
            bpath_(),
            path_(),
            mode_(mode),
            mtime_(mtime),
            type_(type),
            size_(),
            deleted_file_(false),
            fstream_() {
  bpath_ = boost::filesystem::path(constructPath(box_path, path));
  checkArguments(path, type, create);

  std::copy(path.begin(), path.end(), path_.begin());

  if (create)
    storeMetadata();
}
File::File(const std::string& box_path,
           Hash* box_hash,
           const std::string& path,
           const File& file,
           const bool create) :
            box_path_(box_path),
            box_hash_(box_hash),
            bpath_(),
            path_(),
            mode_(),
            mtime_(),
            type_(),
            size_(),
            deleted_file_(false),
            fstream_() {
  bpath_ = boost::filesystem::path(constructPath(box_path, path));
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
File::File(const std::string& box_path,
           Hash* box_hash,
           const std::string& path) :
            box_path_(box_path),
            box_hash_(box_hash),
            bpath_(),
            path_(),
            mode_(),
            mtime_(),
            type_(),
            size_(),
            deleted_file_(false),
            fstream_() {
  bpath_ = boost::filesystem::path(constructPath(box_path, path));

  boost::system::error_code ec;
  if (boost::filesystem::exists(bpath_, ec)) {
    mtime_ = boost::filesystem::last_write_time(bpath_);
    boost::filesystem::file_status stat = boost::filesystem::status(bpath_);
    mode_ = stat.permissions();
    type_ = stat.type();
    if (boost::filesystem::is_regular_file(bpath_))
      size_ = boost::filesystem::file_size(bpath_);
  } else {
    throw boost::filesystem::filesystem_error("", bpath_, ec);
  }

  checkArguments(path, type_, false);

  std::copy(path.begin(), path.end(), path_.begin());
}
File::File(const std::string& box_path,
           Hash* box_hash,
           const std::string& path,
           const bool create,
           const bool deleted_file) :
            box_path_(box_path),
            box_hash_(box_hash),
            bpath_(),
            path_(),
            mode_(),
            mtime_(),
            type_(),
            size_(),
            deleted_file_(deleted_file),
            fstream_() {
  std::copy(path.begin(), path.end(), path_.begin());
  (void)create;  // suppressing warning about not using variable
}
File::~File() {
  if (fstream_.is_open())
    fstream_.close();
}

void File::checkArguments(const std::string& path,
                          const boost::filesystem::file_type type,
                          const bool create) const {
  // check if the path is not too long
  if (path.length() > F_MAXIMUM_PATH_LENGTH) {
    std::stringstream error_msg;
    error_msg << "File path too long. "
      << "Currently supports paths up to " << F_MAXIMUM_PATH_LENGTH
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
bool File::isToBeDeleted() const {
  return deleted_file_;
}
bool File::exists() const {
  return boost::filesystem::exists(bpath_);
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

void File::resize(uint64_t const size) {
  if (type_ == boost::filesystem::regular_file) {
    size_ = size;
    boost::system::error_code ec;
    boost::filesystem::resize_file(bpath_, size_, ec);
    if (ec != 0)
      throw boost::filesystem::filesystem_error("", bpath_, ec);
  }
}
void File::resize() {
  resize(size_);
}
void File::create() {
  if (type_ == boost::filesystem::regular_file) {
    openFile();
  } else if (type_ == boost::filesystem::directory_file) {
    boost::filesystem::create_directory(bpath_);
  } else {
    throw std::range_error("Tried to create file of unsupported filetype. ");
  }
}

void File::openFile() {
  fstream_.open(bpath_.string(),
    std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  fstream_.unsetf(std::ios_base::skipws);
}
void File::closeFile() {
  fstream_.close();
}

uint64_t File::readFileData(char* data,
                           const uint64_t size,
                           uint64_t offset,
                           bool* more) {
  if (deleted_file_) return 0;

  if (!fstream_.is_open())
    openFile();

  uint64_t data_size;
  if (size_ <= F_MAXIMUM_FILE_PACKAGE_SIZE && size_ <= size) {
    fstream_.seekg(offset, std::ios_base::beg);
    fstream_.read(data, size_);
  } else if (size < F_MAXIMUM_FILE_PACKAGE_SIZE) {
    fstream_.seekg(offset, std::ios_base::beg);
    fstream_.read(data, size);
  } else {
    fstream_.seekg(offset, std::ios_base::beg);
    fstream_.read(data, F_MAXIMUM_FILE_PACKAGE_SIZE);
  }
  data_size = fstream_.gcount();

  if (offset + data_size < size_) {
    *more = true;
  } else {
    *more = false;
  }

  return data_size;
}
uint64_t File::readFileData(char* data, uint64_t offset) {
  return readFileData(data, F_MAXIMUM_FILE_PACKAGE_SIZE, offset, new bool());
}
uint64_t File::readFileData(char* data, uint64_t offset, bool* more) {
  return readFileData(data, F_MAXIMUM_FILE_PACKAGE_SIZE, offset, more);
}

void File::storeFileData(const char* data,
                         const int64_t size,
                         const int64_t offset) {
  if (deleted_file_) return;

  if (!fstream_.is_open()) {
    openFile();
  }

  if ( static_cast<int64_t>(size_) <= F_MAXIMUM_FILE_PACKAGE_SIZE
    && static_cast<int64_t>(size_) <= size) {
    fstream_.seekp(offset);
    fstream_.write(data, size_);
  } else if (size < F_MAXIMUM_FILE_PACKAGE_SIZE) {
    fstream_.seekp(offset);
    fstream_.write(data, size);
  } else {
    fstream_.seekp(offset);
    fstream_.write(data, F_MAXIMUM_FILE_PACKAGE_SIZE);
  }

  boost::filesystem::last_write_time(bpath_, static_cast<time_t>(mtime_));
}
void File::storeFileData(const char* data,
                         const uint64_t size,
                         const uint64_t offset) {
  storeFileData(data,
                static_cast<int64_t>(size),
                static_cast<int64_t>(offset));
}

const std::string File::constructPath(const std::string box_path,
                                      const std::string path) const {
  std::string complete_path;
  if (box_path.back() != '/' && path.front() != '/') {
    complete_path = box_path + '/' + path;
  } else if (box_path.back() == '/' && path.front() == '/') {
    std::string box_path_temp = box_path;
    box_path_temp.pop_back();
    complete_path = box_path_temp + path;
  } else {
    complete_path = box_path + path;
  }
  return complete_path;
}

std::ostream& operator<<(std::ostream& ostream, const File& f) {
  ostream.write(reinterpret_cast<char*>(
                  const_cast<unsigned char*>(f.box_hash_->getBytes())
                ), F_GENERIC_HASH_LEN);

  std::string path(f.path_.begin(), f.path_.end());
  ostream.write(path.c_str(), F_MAXIMUM_PATH_LENGTH);

  if (f.deleted_file_) {
    ostream << "IN_DELETE\0";
    return ostream;
  }

  uint16_t mode = htobe16(f.mode_);
  ostream.write((const char*)&mode, 2);

  uint8_t type = uint8_t(f.type_);
  ostream.write((const char*)&type, 1);

  uint32_t mtime = htobe32(f.mtime_);
  ostream.write((const char*)&mtime, 4);

  if (f.type_ == boost::filesystem::regular_file) {
    uint64_t size = htobe64(f.size_);
    char* size_char = new char[8];
    std::memcpy(size_char, &size, 8);
    ostream.write(size_char, 8);
  } else {
    uint64_t size = htobe64(0);
    char* size_char = new char[8];
    std::memcpy(size_char, &size, 8);
    ostream.write(size_char, 8);
  }

  return ostream;
}

std::istream& operator>>(std::istream& istream, File& f) {
  if (f.box_hash_ == nullptr || f.box_path_.length() == 0)
    throw std::out_of_range("Box info not found, File object probably not correctly initialised.");

  char* path_c = new char[F_MAXIMUM_PATH_LENGTH];
  istream.read(path_c, F_MAXIMUM_PATH_LENGTH);
  for (int i = 0; i < F_MAXIMUM_PATH_LENGTH; ++i) {
    f.path_[i] = path_c[i];
  }
  std::string path(f.path_.begin(), f.path_.end());

  std::string deleted;
  istream >> deleted;
  if (deleted == "IN_DELETE") {
    f.deleted_file_ = true;
    f.bpath_ = boost::filesystem::path(f.constructPath(f.box_path_, path));
    if (boost::filesystem::exists(f.bpath_)) {
      boost::system::error_code ec;
      boost::filesystem::remove(f.bpath_, ec);
      if (ec != 0)
        throw boost::filesystem::filesystem_error("", f.bpath_, ec);
    }
    return istream;
  } else {
    istream.seekg(-1*deleted.length(), std::ios_base::cur);
  }

  f.bpath_ = boost::filesystem::path(f.constructPath(f.box_path_, path));

  char* mode_c = new char[2];
  istream.read(mode_c, 2);
  uint16_t mode;
  std::memcpy(&mode, mode_c, 2);
  f.mode_ = boost::filesystem::perms(be16toh(mode));

  char* type_c = new char[1];
  istream.read(type_c, 1);
  uint8_t type;
  std::memcpy(&type, type_c, 1);
  f.type_ = boost::filesystem::file_type(type);

  char* mtime_c = new char[4];
  istream.read(mtime_c, 4);
  uint32_t mtime;
  std::memcpy(&mtime, mtime_c, 4);
  f.mtime_ = be32toh(mtime);

  f.checkArguments(path, f.type_, true);

  if (f.type_ == boost::filesystem::regular_file) {
    char* size_c = new char[8];
    istream.read(size_c, 8);
    uint64_t size;
    std::memcpy(&size, size_c, 8);
    f.size_ = be64toh(size);
  }
  return istream;
}
