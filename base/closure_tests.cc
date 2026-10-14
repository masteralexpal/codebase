/**
 * @file closure_tests.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-03-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <iostream>
#include <type_traits>

#include "base/closure.h"
#include <gtest/gtest.h>

int PlainFunction1(int a, int b) {
  std::cout << a << " + " << b << " = " << a + b;
  return a + b;
}

TEST(TestPlainfunction, Case0) {
  auto closure = base::MakeClosure(PlainFunction1, 4, 2);
  auto result = std::move(closure)();
  std::cout << (std::is_same<decltype(result), int &>::value);
  std::invoke_result<int (&)(int, int), int, int>::type a = 1;
  std::cout << a;
}