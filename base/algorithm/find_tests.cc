#include <gtest/gtest.h>
#include <cstddef>
#include "base/algorithm/find.h"

namespace base {
TEST(Find, case0) {
  std::size_t index = base::Find("abc", 'c');
  ASSERT_EQ(index, 2ULL);
}

TEST(Find, case1) {
  std::size_t index = base::Find("abc", 'a');
  ASSERT_EQ(index, 0ULL);
}

TEST(Find, case2) {
  std::size_t index = base::Find("abc", 'z');
  ASSERT_EQ(index, static_cast<std::size_t>(-1));
}

TEST(FindBack, case0) {
  std::size_t index = base::FindBack("const T (&array)[N]", 'T');
  ASSERT_EQ(index, 6ULL);
}

TEST(FindBack, case1) {
  std::size_t index = base::FindBack("const T (&array)[N]", 'c');
  ASSERT_EQ(index, 0ULL);
}

TEST(FindBack, case2) {
  std::size_t index = base::FindBack("const T (&array)[N]", 'z');
  ASSERT_EQ(index, static_cast<std::size_t>(-1));
}
}  // namespace base