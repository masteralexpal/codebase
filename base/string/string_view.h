/**
 * @file string_piece.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_STRING_STRING_VIEW_H_
#define BASE_STRING_STRING_VIEW_H_

#include <limits>

#include "base/container/internal/iterator.h"

namespace base {
namespace string {

class StringView {
 public:
  static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

  StringView(std::nullptr_t) = delete;

  template <std::size_t N>
  constexpr StringView(char const (&literal)[N]) noexcept
      : cstr_{&literal}, size_(N), begin_(0), end_(0) {}

  constexpr StringView(char const* str, std::size_t size) noexcept
      : cstr_(str), size_(size), begin_(0), end_(size_ - 1) {}

  constexpr std::size_t FindFirstOf(char c) noexcept {
    for (auto i = 0U; i < size_; ++i) {
      if (c == cstr_[i])
        return i;
    }
    return npos;
  }

  constexpr std::size_t FindLastOf(char c) noexcept {
    for (auto i = size_; i != 0U; --i) {
      if (cstr_[i] == c) {
        return i;
      }
    }
    return npos;
  }

  constexpr void RemovePrefix(std::size_t n) noexcept { begin_ = n; }

  constexpr void RemoteSuffix(std::size_t n) noexcept { end_ = n; }

 private:
  char const* cstr_;
  std::size_t size_;
  std::size_t begin_;
  std::size_t end_;
};

}  // namespace string
}  // namespace base
#endif  // BASE_STRINGS_STRING_VIEW_H_