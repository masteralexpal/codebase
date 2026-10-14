/**
 * @file ebo_tests.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <memory>
#include <type_traits>

#include <gtest/gtest.h>

struct Empty {};

struct Foo {
  Empty empty;
};

TEST(Empty, Size) {
  Empty empty;
  Foo foo;
  ASSERT_NE(sizeof(Empty), 0u);
  ASSERT_NE(sizeof(empty), 0u);     // complete objects.
  ASSERT_NE(sizeof(foo.empty), 0u); // member subobjects.
}

struct Bar {
  Empty empty; // 1
  // padding 3
  int i; // 4
};

TEST(Empty, Alignment) {
  ASSERT_NE(sizeof(Bar), 5u); // not equal to 5u.
  ASSERT_EQ(sizeof(Bar), 8u); // 4-byte aligned.
}

TEST(Allocator, Empty) {
  ASSERT_EQ((std::is_empty<std::allocator<int>>::value), true);
}

template <typename T, typename Allocator = std::allocator<T>> class Vector {
public:
  Vector() { begin_ = allocator_.allocate(10); }

  ~Vector() { allocator_.deallocate(begin_, 1); }

private:
  Allocator allocator_;
  T *begin_;
  T *last_;
  T *end_;
};

template <typename T, typename Allocator = std::allocator<T>>
class VectorEBO : public Allocator {
public:
  VectorEBO() { begin_ = this->allocate(10); }

  ~VectorEBO() { this->deallocate(begin_, 1); }

private:
  T *begin_;
  T *last_;
  T *end_;
};

TEST(EBO, Size) {
  Vector<int> vector;
  VectorEBO<int> vector_ebo;
  std::cout << sizeof(vector) << std::endl;
  std::cout << sizeof(vector_ebo) << std::endl;
}
