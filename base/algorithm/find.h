/**
 * @file find.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef BASE_ALGORITHM_FIND_H_
#define BASE_ALGORITHM_FIND_H_
#include <cstddef>
namespace base {

/**
 * @brief ind the index of given value.
 *
 * @tparam T
 * @tparam N
 * @param value
 * @return std::size_t constexpr
 */
template <typename T, std::size_t N>
std::size_t constexpr Find(T const (&array)[N], T const& value) {
  for (std::size_t i = 0; i < N; ++i) {
    if (array[i] == value) {
      return i;
    }
  }
  return static_cast<std::size_t>(-1);
}

/**
 * @brief Find the index of given value from back.
 *
 * @tparam T
 * @tparam N
 * @param value
 * @return std::size_t constexpr
 */
template <typename T, std::size_t N>
std::size_t constexpr FindBack(T const (&array)[N], T const& value) {
  for (std::size_t i = N - 1; i != static_cast<std::size_t>(-1); --i) {
    if (array[i] == value) {
      return i;
    }
  }
  return static_cast<std::size_t>(-1);
}

/**
 * @brief Find the index of given value.
 *
 * @tparam T
 * @param data
 * @param size
 * @param value
 * @return std::size_t constexpr
 */
template <typename T>
std::size_t constexpr Find(T const* data, std::size_t size, T const& value) {
  for (std::size_t i = 0; i < size; ++i) {
    if (data[i] == value) {
      return i;
    }
  }
  return static_cast<std::size_t>(-1);
}

/**
 * @brief Find the index of given value from back.
 *
 * @tparam T
 * @param data
 * @param size
 * @param value
 * @return std::size_t constexpr
 */
template <typename T>
std::size_t constexpr FindBack(T const* data,
                               std::size_t size,
                               T const& value) {
  for (std::size_t i = size - 1; i != static_cast<std::size_t>(-1); --i) {
    if (data[i] == value) {
      return i;
    }
  }
  return static_cast<std::size_t>(-1);
}
}  // namespace base

#endif  // BASE_ALGORITHM_FIND_H_