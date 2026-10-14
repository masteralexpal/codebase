/**
 * @file timestamp_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/timestamp.h"

#include <gtest/gtest.h>
#include <iostream>

namespace base {
TEST(Timestamp, case0) {
  Timestamp timestamp;
  std::cout << timestamp.ToString() << std::endl;
  std::cout << timestamp.ToFormattedString() << std::endl;
}

TEST(Timestamp, case1) {
  Timestamp timestamp = Timestamp::Now();
  std::cout << timestamp.ToString() << std::endl;
  std::cout << timestamp.ToFormattedString() << std::endl;
}

}; // namespace base