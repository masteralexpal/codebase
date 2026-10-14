/**
 * @file simple_allocator.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_ALLOCATOR_SIMPLE_ALLOCATOR_H_
#define BASE_ALLOCATOR_SIMPLE_ALLOCATOR_H_

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <tuple>

namespace base {
template <typename T>
class SimpleAllocator final {
 public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  constexpr SimpleAllocator() noexcept = default;
  constexpr SimpleAllocator(SimpleAllocator const& other) noexcept = default;
  ~SimpleAllocator() = default;

  template <typename U>
  constexpr SimpleAllocator(SimpleAllocator<U> const& other) noexcept {
    std::ignore(other);
  }

  [[nodiscard]] constexpr T* allocate(std::size_t n) {
    return static_cast<T*>(::operator new(n * sizeof(T)));
  }

  constexpr void deallocate(T* p, std::size_t n = 1) {
    std::ignore = n;
    ::operator delete(p);
  }
};

template <class T, class U>
bool operator==(SimpleAllocator<T> const& lhs,
                SimpleAllocator<U> const& rhs) noexcept {
  std::ignore(lhs);
  std::ignore(rhs);
  return true;
}

template <class T, class U>
bool operator!=(SimpleAllocator<T> const& lhs,
                SimpleAllocator<U> const& rhs) noexcept {
  std::ignore(lhs);
  std::ignore(rhs);
  return false;
}
}  // namespace base
#endif