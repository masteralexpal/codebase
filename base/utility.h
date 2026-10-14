/**
 * @file utility.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-28
 *
 * @copyright Copyright (c) 2023
 *
 */

namespace base {
template <typename Type>
Type* AddressOf(Type& value) {
  return reinterpret_cast<Type*>(
      &const_cast<char&>(reinterpret_cast<char const volatile&>(value)));
}

template <typename Type>
Type const* AddressOf(Type const& value) {
  return reinterpret_cast<Type const*>(
      &const_cast<char&>(reinterpret_cast<char const volatile&>(value)));
}

template <typename Type>
Type volatile* AddressOf(Type volatile& value) {
  return reinterpret_cast<Type volatile*>(
      &const_cast<char&>(reinterpret_cast<char const volatile&>(value)));
}

template <typename Type>
Type const volatile* AddressOf(Type const volatile& value) {
  return reinterpret_cast<Type const volatile*>(
      &const_cast<char&>(reinterpret_cast<char const volatile&>(value)));
}

}  // namespace base
