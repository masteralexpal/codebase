/**
 * @file vector_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/allocator/simple_allocator.h"
#include "base/container/vector.h"

#include <gtest/gtest.h>

class VectorTestHelper : public testing::Test {
 public:
  void SetUp() override {};
  void TearDown() override {};
};

TEST_F(VectorTestHelper, Constructor_1) {
  base::Vector<int> vector;
  ASSERT_EQ(vector.Size(), 0ULL);
  ASSERT_EQ(vector.Capacity(), 0ULL);
  ASSERT_EQ(vector.Data(), nullptr);
  ASSERT_EQ(vector.begin(), vector.end());
  ASSERT_EQ(vector.cbegin(), vector.cend());
  ASSERT_EQ(vector.rbegin(), vector.rend());
  ASSERT_EQ(vector.crbegin(), vector.crend());
  ASSERT_EQ(vector.begin(), vector.rend().base());
  ASSERT_EQ(vector.cbegin(), vector.crend().base());
}

TEST_F(VectorTestHelper, Constructor_2) {
  base::Vector<int> vector(base::Allocator<int>{});
  ASSERT_EQ(vector.Size(), 0ULL);
  ASSERT_EQ(vector.Capacity(), 0UL);
  ASSERT_EQ(vector.Data(), nullptr);
  ASSERT_EQ(vector.begin(), vector.end());
  ASSERT_EQ(vector.cbegin(), vector.cend());
  ASSERT_EQ(vector.rbegin(), vector.rend());
  ASSERT_EQ(vector.crbegin(), vector.crend());
  ASSERT_EQ(vector.begin(), vector.rend().base());
  ASSERT_EQ(vector.cbegin(), vector.crend().base());
}

TEST_F(VectorTestHelper, Constructor_3) {
  base::Vector<int> vector(10, 10);
  ASSERT_EQ(vector.Capacity(), 10ULL);
  ASSERT_EQ(vector.Size(), 10ULL);
  for (auto&& i : vector) {
    ASSERT_EQ(i, 10);
  }
}

TEST_F(VectorTestHelper, Constructor_4) {
  base::Vector<int> vector(10);
  ASSERT_EQ(vector.Capacity(), 10ULL);
  ASSERT_EQ(vector.Size(), 10ULL);
  for (auto&& i : vector) {
    ASSERT_EQ(i, 0);
  }
}

TEST_F(VectorTestHelper, Constructor_5) {
  std::vector<int> expect;
  for (auto i = 0; i < 10; ++i) {
    expect.push_back(i);
  }

  base::Vector<int> result{expect.begin(), expect.end()};

  for (std::size_t idx = 0; idx < expect.size(); ++idx) {
    ASSERT_EQ(result[idx], expect[idx]);
  }

  base::Vector<int> other{result.begin(), result.end()};
  for (std::size_t idx = 0; idx < expect.size(); ++idx) {
    ASSERT_EQ(other[idx], expect[idx]);
  }
}

TEST_F(VectorTestHelper, Constructor_6) {
  base::Vector<int> expect;
  for (auto i = 0; i < 10; ++i) {
    expect.PushBack(i);
  }

  base::Vector<int> result{expect};

  ASSERT_EQ(result.Size(), expect.Size());
  ASSERT_EQ(result.Capacity(), expect.Capacity());
  for (std::size_t idx = 0; idx < expect.Size(); ++idx) {
    ASSERT_EQ(result[idx], expect[idx]);
  }
}

TEST_F(VectorTestHelper, Constructor_7) {
  base::Vector<int> expect;
  for (auto i = 0; i < 10; ++i) {
    expect.PushBack(i);
  }

  base::Vector<int> result(expect, base::Allocator<int>());
  ASSERT_EQ(result.Size(), expect.Size());
  ASSERT_EQ(result.Capacity(), expect.Capacity());

  for (std::size_t idx = 0; idx < expect.Size(); ++idx) {
    ASSERT_EQ(result[idx], expect[idx]);
  }

  base::Vector<int> other{result.begin(), result.end()};
  for (std::size_t idx = 0; idx < expect.Size(); ++idx) {
    ASSERT_EQ(other[idx], expect[idx]);
  }
}

TEST_F(VectorTestHelper, Constructor_8) {
  base::Vector<int> expect;
  for (auto i = 0; i < 10; ++i) {
    expect.PushBack(i);
  }

  base::Vector<int> result(std::move(expect));

  ASSERT_EQ(expect.Size(), 0ULL);
  ASSERT_EQ(expect.Capacity(), 0ULL);
  ASSERT_EQ(expect.Data(), nullptr);
  ASSERT_EQ(expect.Empty(), true);
}

TEST_F(VectorTestHelper, Constructor_9) {
  base::Vector<int> expect;
  for (auto i = 0; i < 10; ++i) {
    expect.PushBack(i);
  }

  base::Vector<int> result(std::move(expect), base::Allocator<int>());

  ASSERT_EQ(expect.Size(), 0ULL);
  ASSERT_EQ(expect.Capacity(), 0ULL);
  ASSERT_EQ(expect.Data(), nullptr);
  ASSERT_EQ(expect.Empty(), true);
}

TEST_F(VectorTestHelper, Constructor_10) {
  base::Vector<int> result{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  for (int i = 0; i < 10; ++i) {
    ASSERT_EQ(result[i], i);
  }

  ASSERT_EQ(result.Size(), 10ULL);
  ASSERT_EQ(result.Capacity(), 10ULL);
  ASSERT_NE(result.Data(), nullptr);
  ASSERT_FALSE(result.Empty());
}

TEST_F(VectorTestHelper, Iterator) {
  base::Vector<int> result;
  std::vector<int> expect;
  int array[] = {1, 2, 3, 4, 5};
  for (auto i = 0ULL; i < sizeof(array); ++i) {
    result.PushBack(i);
    expect.push_back(i);
  }
  auto iter1 = result.begin();
  auto iter2 = expect.begin();
  for (auto i = 0ULL; i < sizeof(array); ++i) {
    ASSERT_EQ(*iter1, *iter2);
    ++iter1;
    ++iter2;
  }

  ASSERT_LT(expect.begin(), expect.end());
  ASSERT_LT(expect.cbegin(), expect.cend());
  ASSERT_LT(expect.rbegin(), expect.rend());
  ASSERT_LT(expect.crbegin(), expect.crend());

  ASSERT_LT(result.begin(), result.end());
  ASSERT_LT(result.cbegin(), result.cend());
  ASSERT_LT(result.rbegin(), result.rend());
  ASSERT_LT(result.crbegin(), result.crend());

  ASSERT_GT(expect.end(), expect.begin());
  ASSERT_GT(expect.cend(), expect.cbegin());
  ASSERT_GT(expect.rend(), expect.rbegin());
  ASSERT_GT(expect.crend(), expect.crbegin());

  ASSERT_GT(result.end(), result.begin());
  ASSERT_GT(result.cend(), result.cbegin());
  ASSERT_GT(result.rend(), result.rbegin());
  ASSERT_GT(result.crend(), result.crbegin());

  ASSERT_EQ((result.begin() - result.end()), (expect.begin() - expect.end()));
  ASSERT_EQ((result.cbegin() - result.cend()),
            (expect.cbegin() - expect.cend()));
}

TEST_F(VectorTestHelper, ReverseIterator) {
  base::Vector<int> result;
  std::vector<int> expect;
  int array[] = {1, 2, 3, 4, 5};
  for (auto i = 0ULL; i < sizeof(array); ++i) {
    result.PushBack(i);
    expect.push_back(i);
  }
  auto iter1 = result.rbegin();
  auto iter2 = expect.rbegin();
  for (auto i = 0ULL; i < sizeof(array); ++i) {
    ASSERT_EQ(*iter1, *iter2);
    ++iter1;
    ++iter2;
  }

  ASSERT_EQ(iter1, result.rend());
  ASSERT_EQ(iter2, expect.rend());
  std::vector<int> vector;
}

TEST_F(VectorTestHelper, Temporary) {}