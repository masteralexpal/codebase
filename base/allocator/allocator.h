/**
 * @file allocator.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-02
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_ALLOCATOR_ALLOCATOR_H_
#define BASE_ALLOCATOR_ALLOCATOR_H_

#include <cstddef>
#include <limits>
#include <memory>
#include <tuple>
#include <type_traits>

#include "base/cxx.h"

namespace base {
/**
 * @brief
 *
 * @tparam T
 */
template <typename T>
class Allocator {
 public:
  // I am immortal.
  using value_type = T;

#if UNTIL(CXX_20)
  using pointer = T*;
  using const_pointer = T const*;
  using reference = T&;
  using const_reference = T const&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  template <typename U>
  struct rebind {
    typedef Allocator<U> other;
  };
#endif

  using propagate_on_container_move_assignment = std::true_type;
  using is_always_equal = std::true_type;

#if UNTIL(CXX_11)
  Allocator() throw() {}
  Allocator(Allocator const& other) throw() {}
  template <typename U>
  Allocator(Allocator<U> const& other) throw() {}
#endif

#if SINCE(CXX_11) && UNTIL(CXX_20)
  Allocator() noexcept = default;
  Allocator(Allocator const& other) noexcept = default;
  template <typename U>
  Allocator(Allocator<U> const& other) noexcept {}
#endif

#if SINCE(CXX_20)
  constexpr Allocator() noexcept = default;
  constexpr Allocator(Allocator const& other) noexcept = default;
  template <typename U>
  constexpr Allocator(Allocator<U> const& other) noexcept {
    std::ignore(other);
  }
#endif

#if UNTIL(CXX_20)
  ~Allocator() = default;
#elif SINCE(CXX_20)
  constexpr ~Allocator() = default;
#endif

// address
#if UNTIL(CXX_11)
  pointer address(reference x) const { return std::addressof(x); }
  const_pointer address(const_reference x) const { return std::addressof(x); }
#elif SINCE(CXX_11) && UNTIL(CXX_20)
  pointer address(reference x) const noexcept { return std::addressof(x); }
  const_pointer address(const_reference x) const noexcept {
    return std::addressof(x);
  }
#endif

// allocate
#if UNTIL(CXX_17)
  pointer allocate(size_type n, void const* hint = 0) {
#if SINCE(CXX_11)
    static_assert(sizeof(T) != 0, "Cannot allocate incomplete types.");
#endif  // SINCE(CXX_11)
    if (n > (std::size_t(-1) / sizeof(T))) {
      throw std::bad_array_new_length();
    }
    return static_cast<T*>(::operator new(n * sizeof(T)));
  }
#elif SINCE(CXX_17) && UNTIL(CXX_20)
  T* allocate(std::size_t n, void const* hint) {
#if SINCE(CXX_11)
    static_assert(sizeof(T) != 0, "Cannot allocate incomplete types.");
#endif  // SINCE(CXX_11)
    if (n > (std::size_t(-1) / sizeof(T))) {
      throw std::bad_array_new_length();
    }
    return static_cast<T*>(::operator new(n * sizeof(T)));
  }

  T* allocate(std::size_t n) { return allocate(n, nullptr); }
#elif SINCE(CXX_20)
  [[nodiscard]] constexpr T* allocate(std::size_t n) {
#if SINCE(CXX_11)
    static_assert(sizeof(T) != 0, "Cannot allocate incomplete types.");
#endif
    if (n > (std::size_t(-1) / sizeof(T))) {
      throw std::bad_array_new_length();
    }
    return static_cast<T*>(::operator new(n * sizeof(T)));
  }
#endif  // UNTIL(CXX_17)

// deallocate
#if UNTIL(CXX_20)
  void deallocate(T* p, std::size_t n) {
    (void)n;
    ::operator delete(p);
  }
#elif SINCE(CXX_20)
  constexpr void deallocate(T* p, std::size_t n) {
    (void)n;
    ::operator delete(p);
  }
#endif

// max_size
#if UNTIL(CXX_11)
  std::size_t max_size() const throw() {
    return std::numeric_limits<size_t>::max() / sizeof(value_type);
  }
#elif SINCE(CXX_11) && UNTIL(CXX_20)
  size_type max_size() const noexcept {
    return std::numeric_limits<size_t>::max() / sizeof(value_type);
  }
#endif

// construct
#if UNTIL(CXX_11)
  void construct(pointer p, const_reference value) {
    ::new ((void*)p) T(value);
  }
#elif SINCE(CXX_11) && UNTIL(CXX_20)
  template <typename U, typename... Args>
  void construct(U* p, Args&&... args) {
    ::new ((void*)p) U(std::forward<Args>(args)...);
  }
#endif

// destroy
#if UNTIL(CXX_11)
  void destroy(pointer p) { p->~T(); }
#elif SINCE(CXX_11) && UNTIL(CXX_20)
  template <typename U>
  void destroy(U* p) {
    p->~U();
  }
#endif
};

#if UNTIL(CXX_17)
template <>
class Allocator<void> {
 public:
  using reference = T&;
  using const_reference = T const&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
};
#endif

template <typename T, typename U>
inline bool operator==(Allocator<T> const& lhs, Allocator<U> const& rhs) {
  std::ignore = lhs;
  std::ignore = rhs;
  return true;
}

#if UNTIL(CXX_17)
template <typename T, typename U>
inline bool operator!=(Allocator<T> const& lhs, Allocator<U> const& rhs) {
  return false;
}
#endif

}  // namespace base
#endif  // BASE_ALLOCATOR_ALLOCATOR_H_