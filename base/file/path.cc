/**
 * @file path.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/file/path.h"

#include <string>
#include <utility>

namespace base {
Path::Path(char const* path_name) : path_name_(path_name) {}

Path::Path(std::string path_name) : path_name_(path_name) {}

Path::Path(Path const& other) : path_name_(other.path_name_) {}

Path& Path::operator=(Path const& other) {
  if (this != &other) {
    Path _(other);
    std::swap(_.path_name_, path_name_);
  }
  return *this;
}

Path::Path(Path&& other) noexcept : path_name_(std::move(other.path_name_)) {}

Path& Path::operator=(Path&& other) noexcept {
  path_name_ = std::move(other.path_name_);
  return *this;
}

bool Path::Empty() const noexcept {
  return path_name_.empty();
}

bool Path::IsAbsolute() const noexcept {
  if (Empty()) {
    return false;
  }
  return path_name_.front() == GetPlatformSeprator();
}

void Path::Append(char const* sub_path) {
  // Appending to an empty path yields a relative path (std::filesystem
  // semantics); back() on an empty string is undefined behavior.
  if (!path_name_.empty() && path_name_.back() != GetPlatformSeprator()) {
    path_name_.push_back(GetPlatformSeprator());
  }
  path_name_.append(sub_path);
}

void Path::Append(std::string const& sub_path) {
  if (!path_name_.empty() && path_name_.back() != GetPlatformSeprator()) {
    path_name_.push_back(GetPlatformSeprator());
  }
  path_name_.append(sub_path);
}

void Path::Append(Path const& sub_path) {
  if (!path_name_.empty() && path_name_.back() != GetPlatformSeprator()) {
    path_name_.push_back(GetPlatformSeprator());
  }
  path_name_.append(sub_path.path_name_);
}

std::string Path::ToString() const {
  return path_name_;
}
}  // namespace base