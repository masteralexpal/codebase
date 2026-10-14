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

#include <string>

#include <gtest/gtest.h>

namespace base {

TEST(Path, Construction0) {
  char const sep = base::Path::GetPlatformSeprator();
  base::Path path;
  ASSERT_EQ(path.Empty(), true);
  ASSERT_EQ(path.IsAbsolute(), false);
  path.Append(Path{"tmp"});
  ASSERT_EQ(path.ToString(), "tmp");
  path.Append("hikari");
  ASSERT_EQ(path.ToString(), std::string{"tmp"} + sep + "hikari");
  path.Append("dora");
  ASSERT_EQ(path.ToString(), std::string{"tmp"} + sep + "hikari" + sep + "dora");
}

}  // namespace base
