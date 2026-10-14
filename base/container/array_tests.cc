/**
 * @file array_tests.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/container/array.h"

#include <gtest/gtest.h>
#include <cstddef>
#include <stdexcept>
#include <type_traits>

namespace base {
TEST(Array, Construcion_0) {
  Array<int, 0> array{};
  ASSERT_EQ(array.begin(), array.end());
  ASSERT_EQ(array.cbegin(), array.cend());
  ASSERT_EQ(array.rbegin(), array.rend());
  ASSERT_EQ(array.crbegin(), array.crend());
  ASSERT_EQ(array.Size(), 0ULL);
  ASSERT_TRUE(array.Empty());
}

TEST(Array, Construcion_1) {
  Array<int, 1> array{};
  ASSERT_NE(array.begin(), array.end());
  ASSERT_NE(array.cbegin(), array.cend());
  ASSERT_NE(array.rbegin(), array.rend());
  ASSERT_NE(array.crbegin(), array.crend());
  ASSERT_EQ(array.Size(), 1ULL);
  ASSERT_FALSE(array.Empty());
}

TEST(Array, Iterators) {
  int expect[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Array<int, 10> array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto iter = array.begin();
  for (auto&& element : expect) {
    ASSERT_EQ(element, *iter);
    ++iter;
  }

  ASSERT_EQ(iter, array.end());
}

TEST(Array, ConstIterators) {
  int expect[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Array<int, 10> array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto iter = array.cbegin();
  for (auto&& element : expect) {
    ASSERT_EQ(element, *iter);
    ++iter;
  }

  ASSERT_EQ(iter, array.cend());
}

TEST(Array, ReverseIterators) {
  int expect[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
  Array<int, 10> array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto iter = array.rbegin();
  for (auto&& element : expect) {
    ASSERT_EQ(element, *iter);
    ++iter;
  }

  ASSERT_EQ(iter, array.rend());
}

TEST(Array, ConstReverseIterators) {
  int expect[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
  Array<int, 10> array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto iter = array.crbegin();
  for (auto&& element : expect) {
    ASSERT_EQ(element, *iter);
    ++iter;
  }

  ASSERT_EQ(iter, array.crend());
}

TEST(Array, At) {
  int expect[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Array<int, 10> array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ASSERT_THROW(array.At(11), std::out_of_range);
  ASSERT_NO_THROW(array.At(5));
  for (std::size_t idx = 0; idx < array.Size(); ++idx) {
    ASSERT_EQ(expect[idx], array.At(idx));
    ASSERT_TRUE((std::is_same_v<int&, decltype(array.At(idx))>));
    ASSERT_FALSE((std::is_same_v<int const&, decltype(array.At(idx))>));
    ASSERT_FALSE((std::is_same_v<int&&, decltype(array.At(idx))>));
    ASSERT_FALSE((std::is_same_v<int, decltype(array.At(idx))>));
  }
}

TEST(Array, ConstAt) {
  int expect[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Array<int, 10> const array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ASSERT_THROW(array.At(11), std::out_of_range);
  ASSERT_NO_THROW(array.At(5));
  for (std::size_t idx = 0; idx < array.Size(); ++idx) {
    ASSERT_EQ(expect[idx], array.At(idx));
    ASSERT_TRUE((std::is_same_v<int const&, decltype(array.At(idx))>));
    ASSERT_FALSE((std::is_same_v<int&, decltype(array.At(idx))>));
    ASSERT_FALSE((std::is_same_v<int&&, decltype(array.At(idx))>));
    ASSERT_FALSE((std::is_same_v<int, decltype(array.At(idx))>));
  }
}

TEST(Array, FrontAndBack) {
  Array<int, 10> array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ASSERT_EQ(array.Front(), 0);
  ASSERT_EQ(array.Back(), 9);
  ASSERT_TRUE((std::is_same_v<int&, decltype(array.Front())>));
  ASSERT_FALSE((std::is_same_v<int const&, decltype(array.Front())>));
  ASSERT_FALSE((std::is_same_v<int&&, decltype(array.Front())>));
  ASSERT_FALSE((std::is_same_v<int, decltype(array.Front())>));

  ASSERT_TRUE((std::is_same_v<int&, decltype(array.Back())>));
  ASSERT_FALSE((std::is_same_v<int const&, decltype(array.Back())>));
  ASSERT_FALSE((std::is_same_v<int&&, decltype(array.Back())>));
  ASSERT_FALSE((std::is_same_v<int, decltype(array.Back())>));
}

TEST(Array, ConstFrontAndBack) {
  Array<int, 10> const array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ASSERT_THROW(array.At(11), std::out_of_range);
  ASSERT_NO_THROW(array.At(5));
}

TEST(Array, Fill) {
  Array<int, 10> array;
  for (auto&& elem : array) {
    ASSERT_EQ(elem, 0);
  }
  array.Fill(1);

  for (auto&& elem : array) {
    ASSERT_EQ(elem, 1);
  }
}

TEST(Array, Swap) {
  Array<int, 2> array0{1, 2};
  Array<int, 2> array1{3, 4};
  array0.Swap(array1);
  ASSERT_EQ(array0[0], 3);
  ASSERT_EQ(array0[1], 4);
  ASSERT_EQ(array1[0], 1);
  ASSERT_EQ(array1[1], 2);
}

}  // namespace base
