/**
 * @file hash_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/algorithm/hash.h"

#include <gtest/gtest.h>

namespace base {
TEST(FNV1A, case0) {
  FNV1A<std::string, std::uint32_t> fnv1a;
  std::string heck = "fnv1a";
  auto hash = fnv1a(heck);
  FNV1A32<char> fnv1a32;
  auto hash2 = fnv1a32(heck);
  ASSERT_EQ(hash, hash2);
}
} // namespace base