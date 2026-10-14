/**
 * @file reference.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cstddef>

template <typename Type, typename Traits>
class Pointer {
 public:
  using value_type = Type;
  using reference = value_type&;
  using const_reference = value_type const&;
  using pointer = value_type*;
  using const_pointer = value_type const*;
  using traits_type = Traits;

  Pointer() noexcept : pointer_(nullptr) {}
  explict Pointer(std::nullptr_t) noexcept : pointer_(nullptr) {}

 private:
  pointer pointer_;
};