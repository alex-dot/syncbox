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
            path_(),
            mode_(),
            mtime_(),
            type_() {}
File::File(const std::string& path, const bool create) :
            path_(),
            mode_(),
            mtime_(),
            type_() {
  checkArguments(path, create);

  boost::filesystem::path bpath(path);
  std::copy(path.begin(), path.end(), path_.begin());

  if (!create) {
    mtime_ = boost::filesystem::last_write_time(bpath);
    boost::filesystem::file_status stat = boost::filesystem::status(bpath);
    mode_ = stat.permissions();
    type_ = stat.type();
  }
}
File::File(const std::string& path,
           const bool create,
           const boost::filesystem::perms mode,
           const int32_t mtime) :
            path_(),
            mode_(mode),
            mtime_(mtime),
            type_() {
  checkArguments(path, create);

  boost::filesystem::path bpath(path);
  std::copy(path.begin(), path.end(), path_.begin());
  boost::filesystem::file_status stat = boost::filesystem::status(bpath);
  type_ = stat.type();

  if (create)
    storeMetadata();
}
File::File(const std::string& path, const File& file, const bool create) :
            path_(),
            mode_(),
            mtime_(),
            type_() {
  checkArguments(path, create);

  boost::filesystem::path bpath(path);
  std::copy(path.begin(), path.end(), path_.begin());
  boost::filesystem::file_status stat = boost::filesystem::status(bpath);
  type_ = stat.type();
  mode_ = file.getMode();
  mtime_ = file.getMtime();

  if (create)
    storeMetadata();
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
  boost::filesystem::path bpath(path);
  if (!boost::filesystem::exists(bpath, ec)) {
    if (!create) {
      throw boost::filesystem::filesystem_error("", bpath, ec);
    } else {
      boost::filesystem::ofstream ofs(bpath);
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

void File::setMode(boost::filesystem::perms mode) {
  mode_ = mode;
}
void File::setMtime(int32_t mtime) {
  mtime_ = mtime;
}

void File::storeMetadata() const {
  std::string path(path_.begin(), path_.end());
  boost::filesystem::path bpath(path);

  boost::filesystem::permissions(bpath, mode_);
  boost::filesystem::last_write_time(bpath, static_cast<time_t>(mtime_));
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

  int p, t;
  istream >> p >> f.mtime_ >> t;
  f.mode_ = boost::filesystem::perms(p);
  f.type_ = boost::filesystem::file_type(t);
  return istream;
}
