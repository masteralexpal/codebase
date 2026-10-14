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

#include <gtest/gtest.h>
#include "base/container/__vector.h"

TEST(Vector, Constructor) {
  base::Vector<int> vector;

  std::cout << sizeof(vector) << std::endl;
}
