/**
 * @file comparator.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_ALGORITHM_COMPARATOR_H_
#define BASE_ALGORITHM_COMPARATOR_H_
namespace base {
template <typename T, typename U>
struct LessThan {
  bool operator()(T const& t, U const& u) { return t < u; }
};

template <typename T, typename U>
struct GreaterThan {
  bool operator()(T const& t, U const& u) { return t > u; }
};

template <typename T, typename U>
struct Equal {
  bool operator()(T const& t, U const& u) { return t == u; }
};

template <typename T, typename U>
struct LessThanOrEqual {
  bool operator()(T const& t, U const& u) { return t <= u; }
};

template <typename T, typename U>
struct GreaterThanOrEqual {
  bool operator()(T const& t, U const& u) { return t >= u; }
};
}  // namespace base
#endif