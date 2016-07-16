/**
 * \file      file.cpp
 * \brief     Small class for condensing file information.
 * \author    Alexander Herr
 * \date      2014-2016
 * \copyright GNU Public License v3 or higher. 
 */

#include "file.hpp"

#include <string>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <iomanip>

File::File() :
            bpath_(),
            path_(),
            mode_(),
            mtime_(),
            type_(),
            size_() {}
File::File(const std::string& path, const bool create) :
            bpath_(),
            path_(),
            mode_(),
            mtime_(),
            type_(),
            size_() {
  bpath_ = boost::filesystem::path(path);
  checkArguments(path, create);

  std::copy(path.begin(), path.end(), path_.begin());

  if (!create) {
    mtime_ = boost::filesystem::last_write_time(bpath_);
    boost::filesystem::file_status stat = boost::filesystem::status(bpath_);
    mode_ = stat.permissions();
    type_ = stat.type();
    size_ = boost::filesystem::file_size(bpath_);
  }
}
File::File(const std::string& path,
           const bool create,
           const boost::filesystem::perms mode,
           const int32_t mtime) :
            bpath_(),
            path_(),
            mode_(mode),
            mtime_(mtime),
            type_(),
            size_() {
  bpath_ = boost::filesystem::path(path);
  checkArguments(path, create);

  std::copy(path.begin(), path.end(), path_.begin());
  boost::filesystem::file_status stat = boost::filesystem::status(bpath_);
  type_ = stat.type();

  if (create)
    storeMetadata();
}
File::File(const std::string& path, const File& file, const bool create) :
            bpath_(),
            path_(),
            mode_(),
            mtime_(),
            type_(),
            size_() {
  bpath_ = boost::filesystem::path(path);
  checkArguments(path, create);

  std::copy(path.begin(), path.end(), path_.begin());
  boost::filesystem::file_status stat = boost::filesystem::status(bpath_);
  type_ = stat.type();
  mode_ = file.getMode();
  mtime_ = file.getMtime();

  if (create) {
    storeMetadata();
    resize(file.getSize());
  }
}
File::~File() {}

void File::checkArguments(const std::string& path, const bool create) const {
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
    if (!create) {
      throw boost::filesystem::filesystem_error("", bpath_, ec);
    } else {
      boost::filesystem::ofstream ofs(bpath_);
      ofs.close();
    }
  }
}

const std::string File::getPath() const {
  std::string path(path_.begin(), path_.end());
  return path;
}
boost::filesystem::perms File::getMode() const {
  return mode_;
}
int32_t File::getMtime() const {
  return mtime_;
}
int64_t File::getSize() const {
  return size_;
}

void File::setMode(boost::filesystem::perms mode) {
  mode_ = mode;
}
void File::setMtime(int32_t mtime) {
  mtime_ = mtime;
}

void File::storeMetadata() const {
  boost::filesystem::permissions(bpath_, mode_);
  boost::filesystem::last_write_time(bpath_, static_cast<time_t>(mtime_));
}

void File::resize(int64_t size) {
  size_ = size;
  boost::system::error_code ec;
  boost::filesystem::resize_file(bpath_, size_, ec);
  if (ec != 0)
    throw boost::filesystem::filesystem_error("", bpath_, ec);
}

int File::readFileData(char* data,
                       int64_t* size,
                       const int64_t offset) const {
  boost::filesystem::ifstream ifs(bpath_);

  int64_t data_size;
  if (size_ <= SB_MAXIMUM_FILE_PACKAGE_SIZE && size_ <= *size) {
    ifs.read(data, size_);
    data_size = size_;
  } else if (*size < SB_MAXIMUM_FILE_PACKAGE_SIZE) {
    ifs.read(data, *size);
    data_size = *size;
  } else {
    ifs.read(data, SB_MAXIMUM_FILE_PACKAGE_SIZE);
    data_size = SB_MAXIMUM_FILE_PACKAGE_SIZE;
  }
  int64_t* new_size = new int64_t(data_size);
  std::swap(size, new_size);
  delete new_size;

  ifs.close();

  if (offset + *size < size_) {
    return 1;
  }
  return 0;
}

void File::storeFileData(const char* data, const int64_t size) const {
  boost::filesystem::ofstream ofs(bpath_);

  if (size_ <= SB_MAXIMUM_FILE_PACKAGE_SIZE && size_ <= size) {
    ofs.write(data, size_);
  } else if (size < SB_MAXIMUM_FILE_PACKAGE_SIZE) {
    ofs.write(data, size);
  } else {
    ofs.write(data, SB_MAXIMUM_FILE_PACKAGE_SIZE);
  }
  ofs.close();

  boost::filesystem::last_write_time(bpath_, static_cast<time_t>(mtime_));
}

std::ostream& operator<<(std::ostream& ostream, const File& f) {
  std::string path(f.path_.begin(), f.path_.end());
  ostream << path << " "
    << std::setfill('0') << std::setw(4)  << f.mode_  << " "
    << std::setfill('0') << std::setw(32) << f.mtime_ << " "
    << std::setfill('0') << std::setw(1)  << f.type_;
  return ostream;
}
std::istream& operator>>(std::istream& istream, File& f) {
  std::string path;
  istream >> path;
  std::array<char, SB_MAXIMUM_PATH_LENGTH> new_path;
  std::copy(path.begin(), path.end(), new_path.begin());
  f.path_.swap(new_path);
  f.bpath_ = boost::filesystem::path(path);

  int p, t;
  istream >> p >> f.mtime_ >> t;
  f.mode_ = boost::filesystem::perms(p);
  f.type_ = boost::filesystem::file_type(t);
  return istream;
}
