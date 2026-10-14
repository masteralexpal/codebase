/**
 * @file lifetime.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-12
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef BASE_HELPER_CONSTRUCT_H_
#define BASE_HELPER_CONSTRUCT_H_

#include <utility>

namespace base {
template <typename T, typename... Args>
void Construct(T* p, Args&&... args) {
  ::new (p) T(std::forward<Args>(args)...);
}

template <typename T>
void Destruct(T* p) {
  p->~T();
}

}  // namespace base
#endif  // BASE_HELPER_CONSTRUCT_H_