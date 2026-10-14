/**
 * @file reference_counting_tests.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/memory/reference_counting.h"

#include <gtest/gtest.h>

namespace base {
class Base : public ReferenceCounting<Base> {
 public:
  virtual int Do() { return 1; }
};

class Derived : public Base {
 public:
  int Do() override { return 2; }
};

TEST(ReferenceCounting, Construction) {
  Base* base = new Derived;
  ASSERT_EQ(base->Do(), 2);
  ASSERT_EQ(base->Count(), 1);
  ASSERT_TRUE(base->Unique());
  ASSERT_FALSE(base->Shared());
  base->Add();
  ASSERT_EQ(base->Count(), 2);
  ASSERT_TRUE(base->Shared());
  ASSERT_FALSE(base->Unique());
  base->Sub();
  ASSERT_EQ(base->Count(), 1);
  ASSERT_TRUE(base->Unique());
  ASSERT_FALSE(base->Shared());
}
}  // namespace base