/**
 * @file source_location_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "base/source_location.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include <gtest/gtest.h>

template <std::size_t N>
constexpr char const* ExtractFileName(char const (&file_path)[N]) {
  auto pos = SIZE_MAX;
  for (auto i = 0U; i < N; ++i) {
    if (file_path[i] == '/') {
      pos = i;
    }
  }
  return &(file_path[pos + 1]);
}

TEST(SourceLocation, Construction) {
  auto file_name = "source_location_tests.cc";
  auto line = __LINE__;
  base::SourceLocation source_location(__FILE__, __LINE__);
  ASSERT_STREQ(source_location.file_name(), file_name);
  ASSERT_EQ(source_location.line(), line + 1);
}

TEST(SourceLocation, CurrentSourceLocation) {
  auto file_name = "source_location_tests.cc";
  auto line = __LINE__;
  CURRNET_SOURCE_LOCATION(heck);
  ASSERT_STREQ(heck.file_name(), file_name);
  ASSERT_EQ(heck.line(), line + 1);
}
