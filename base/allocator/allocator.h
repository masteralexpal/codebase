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
#include <new>
#include <tuple>
#include <type_traits>

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

  using propagate_on_container_move_assignment = std::true_type;
  using is_always_equal = std::true_type;

  constexpr Allocator() noexcept = default;
  constexpr Allocator(Allocator const& other) noexcept = default;
  template <typename U>
  constexpr Allocator(Allocator<U> const& other) noexcept {
    std::ignore = other;
  }

  constexpr ~Allocator() = default;

  [[nodiscard]] constexpr T* allocate(std::size_t n) {
    static_assert(sizeof(T) != 0, "Cannot allocate incomplete types.");
    if (n > (std::size_t(-1) / sizeof(T))) {
      throw std::bad_array_new_length();
    }
    return static_cast<T*>(::operator new(n * sizeof(T)));
  }

  constexpr void deallocate(T* p, std::size_t n) {
    (void)n;
    ::operator delete(p);
  }
};

template <typename T, typename U>
inline bool operator==(Allocator<T> const& lhs, Allocator<U> const& rhs) {
  std::ignore = lhs;
  std::ignore = rhs;
  return true;
}

}  // namespace base
#endif  // BASE_ALLOCATOR_ALLOCATOR_H_
