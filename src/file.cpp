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
File::File(const std::string& path) :
            path_(),
            mode_(),
            mtime_(),
            type_() {
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
        throw boost::filesystem::filesystem_error("", bpath, ec);
    }

    std::copy(path.begin(), path.end(), path_.begin());
    mtime_ = boost::filesystem::last_write_time(bpath);
    boost::filesystem::file_status stat = boost::filesystem::status(bpath);
    mode_ = stat.permissions();
    type_ = stat.type();
}
File::~File() {}

const std::string File::getPath() const {
  std::string path(path_.begin(), path_.end());
  return path;
}

std::ostream& operator<<(std::ostream& ostream, const File& f) {
    std::string path(f.path_.begin(), f.path_.end());
    ostream << path    << " "
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
