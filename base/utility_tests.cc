/**
 * @file utility_tests.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/utility.h"
#include <gtest/gtest.h>

class A {
private:
  friend class B;
  A *operator&() { return this; }
};

class B {
public:
  A *AddressOf(A &a) const volatile noexcept { return &a; }
  A const *AddressOf(A const &a) const volatile noexcept { return &a; }
  A volatile *AddressOf(A volatile &a) const volatile noexcept { return &a; }
  A const volatile *AddressOf(A const volatile &a) const volatile noexcept {
    return &a;
  }
};

TEST(AddressOf, A) {
  A a;
  B b;
  A *expect = b.AddressOf(a);
  A *result = base::AddressOf(a);
  ASSERT_EQ(expect, result);
}

TEST(AddressOf, ConstA) {
  A const ca;
  B b;
  A const *expect = b.AddressOf(ca);
  A const *result = base::AddressOf(ca);
  ASSERT_EQ(expect, result);
}

TEST(AddressOf, VolatileA) {
  A volatile va;
  B b;
  A volatile *expect = b.AddressOf(va);
  A volatile *result = base::AddressOf(va);
  ASSERT_EQ(expect, result);
}

TEST(AddressOf, ConstVolatileA) {
  A const volatile cva;
  B b;
  A const volatile *expect = b.AddressOf(cva);
  A const volatile *result = base::AddressOf(cva);
  ASSERT_EQ(expect, result);
}