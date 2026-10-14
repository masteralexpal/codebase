/**
 * @file tree.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_ALGORITHM_TREE_H_
#define BASE_ALGORITHM_TREE_H_
#include "base/allocator/simple_allocator.h"
namespace base {
template <typename T, typename Allocator = SimpleAllocator<T>>
class RedBlackTree {
 public:
  // STL style operations:
  // TODO
  template <typename U = T>
  class iterator {};
  void Insert(T const& t);
  void Insert(T&& t);
  // STL style operations:
};
}  // namespace base
#endif