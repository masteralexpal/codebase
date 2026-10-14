/**
 * @file __vector_tests.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/container/__vector.h"
#include <gtest/gtest.h>

TEST(Vector, Constructor) {
  base::Vector<int> vector;

  std::cout << sizeof(vector) << std::endl;
}
