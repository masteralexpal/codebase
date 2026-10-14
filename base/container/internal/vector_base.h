/**
 * @file vector_base.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONTAINER_INTERNAL_VECTOR_BASE_H_
#define BASE_CONTAINER_INTERNAL_VECTOR_BASE_H_

#include "base/allocator/allocator.h"

namespace base {
namespace internal {
template <typename Tp>
class VectorBase : public Allocator<Tp> {
 public:
  constexpr VectorBase() noexcept
      : begin_(nullptr), last_(nullptr), end_(nullptr) {}

 private:
  Tp* begin_;
  Tp* last_;
  Tp* end_;
};
}  // namespace internal
}  // namespace base
#endif