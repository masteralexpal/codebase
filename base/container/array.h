/**
 * @file array.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONTAINER_ARRAY_H_
#define BASE_CONTAINER_ARRAY_H_

#include "base/container/internal/iterator.h"

#include <cstddef>
#include <stdexcept>

namespace base {
template <typename T, std::size_t N>
class Array {
 public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = value_type const&;
  using pointer = value_type*;
  using const_pointer = value_type const*;
  using iterator = GenericIterator<pointer>;
  using const_iterator = GenericIterator<const_pointer>;
  using reverse_iterator = GenericReverseIterator<iterator>;
  using const_reverse_iterator = GenericReverseIterator<const_iterator>;

  template <typename... Types>
  Array(Types&&... values) : data_{std::forward<Types>(values)...} {}

  // Element access
  reference At(size_type pos) {
    BoundsCheck(pos);
    return data_[pos];
  }

  const_reference At(size_t pos) const {
    BoundsCheck(pos);
    return data_[pos];
  }

  reference operator[](size_type pos) { return data_[pos]; }

  const_reference operator[](size_type pos) const { return data_[pos]; }

  reference Front() { return data_[0U]; }

  const_reference Front() const { return data_[0U]; }

  reference Back() { return data_[Size() - 1]; }

  const_reference Back() const { return data_[Size() - 1]; }

  pointer Data() noexcept { return data_; }

  const_pointer Data() const noexcept { return data_; }

  iterator begin() noexcept { return iterator(data_); }
  const_iterator begin() const noexcept { return iterator(data_); }

  iterator end() noexcept { return iterator(data_ + Size()); }
  const_iterator end() const noexcept { return iterator(data_ + Size()); }

  const_iterator cbegin() const noexcept { return const_iterator(data_); }

  const_iterator cend() const noexcept {
    return const_iterator(data_ + Size());
  }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  const_reverse_iterator rbegin() const noexcept {
    return reverse_iterator(end());
  }

  const_reverse_iterator rend() const noexcept {
    return reverse_iterator(begin());
  }

  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }

  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  inline constexpr bool Empty() const noexcept { return Size() == 0U; }

  inline constexpr size_type Size() const noexcept { return N; }

  inline constexpr size_type MaxSize() const noexcept {
    return static_cast<size_type>(-1) / sizeof(value_type);
  }

  // operations
  void Fill(const_reference value) {
    for (size_type idx = 0U; idx < Size(); ++idx) {
      data_[idx] = value;
    }
  }

  void Swap(Array& other) {
    if (this != &other) {
      std::swap(data_, other.data_);
    }
  }

  // ~operations

 private:
  inline constexpr void BoundsCheck(size_type index) const {
    if (!(index < Size())) {
      throw std::out_of_range("Index out of range.");
    }
  }

  value_type data_[N];
};
}  // namespace base
#endif  // BASE_CONTAINER_ARRAY_H_