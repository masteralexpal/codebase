/**
 * @file shared_ptr_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <gtest/gtest.h>

#include "base/memory/internal/shared_ptr_impl.h"
namespace base {
TEST(SharedPointer, Construct) {
  base::SharedPtr<int> ptr(new int(5));
}
}  // namespace base