/**
 * @file path_tests.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "base/file/path.h"

#include <gtest/gtest.h>

namespace base {

TEST(Path, Construction0) {
  base::Path path;
  ASSERT_EQ(path.Empty(), true);
  ASSERT_EQ(path.IsAbsolute(), false);
  path.Append(Path{"tmp"});
  ASSERT_EQ(path.ToString(), "/tmp");
  path.Append("hikari/");
  ASSERT_EQ(path.ToString(), "/tmp/hikari/");
  path.Append("dora");
  ASSERT_EQ(path.ToString(), "/tmp/hikari/dora");
}

}  // namespace base
