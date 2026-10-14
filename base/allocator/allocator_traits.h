/**
 * @file allocator_traits.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_ALLOCATOR_ALLOCATOR_TRAITS_H_
#define BASE_ALLOCATOR_ALLOCATOR_TRAITS_H_

#include <memory>

namespace base {
template <typename Allocator>
struct AllocatorTraits {
  using allocator_type = Allocator;
  using value_type = typename allocator_type::value_type;
  using pointer = value_type*;
  using const_pointer =
      typename std::pointer_traits<pointer>::template rebind<value_type const>;
  using void_type =
      typename std::pointer_traits<pointer>::template rebind<void>;
  using const_void_type =
      typename std::pointer_traits<pointer>::template rebind<void const>;
  using reference = value_type&;
  using difference_type =
      typename std::pointer_traits<pointer>::difference_type;
};
}  // namespace base
#endif