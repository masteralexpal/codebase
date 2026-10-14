/**
 * @file source_location.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_SOURCE_LOCATION_H_
#define BASE_SOURCE_LOCATION_H_

#include <cstddef>
#include <cstdint>

#include "base/macro.h"

#if IS_WIN32 || IS_WIN64
constexpr char const kPathSeparator = '\\';
#else
constexpr char const kPathSeparator = '/';
#endif

namespace base {

class SourceLocation {
 public:
  template <std::size_t N>
  inline constexpr SourceLocation(char const (&file_path)[N], int line) noexcept
      : file_name_(ExtractFileName(file_path)), line_(line) {}

  inline char const* file_name() const noexcept { return file_name_; }

  inline int line() const noexcept { return line_; }

 private:
  template <std::size_t N>
  static constexpr char const* ExtractFileName(char const (&file_path)[N]) {
    auto pos = SIZE_MAX;
    for (auto i = 0U; i < N; ++i) {
      if (file_path[i] == kPathSeparator) {
        pos = i;
      }
    }
    return &(file_path[pos + 1]);
  }

  char const* file_name_{nullptr};
  int const line_{0};
};

#define CURRNET_SOURCE_LOCATION(Name) \
  base::SourceLocation Name {         \
    __FILE__, __LINE__                \
  }
}  // namespace base
#endif  // BASE_SOURCE_LOCATION_H_