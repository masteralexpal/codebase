/**
 * @file algo_exp.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <algorithm>

#include <gtest/gtest.h>

class AlgoTestEnviroment : public testing::Environment {};
class AlgoTest : public testing::Test {};

TEST_F(AlgoTest, for_each) {
  std::vector<int> ints{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::stringstream stringstream;

  int count = 0;
  std::for_each(ints.begin(), ints.end(),
                [&stringstream, &count](int value) mutable -> void {
                  stringstream << value << ", ";
                  ++count;
                });

  ASSERT_EQ(stringstream.str(), std::string{"0, 1, 2, 3, 4, 5, 6, 7, 8, 9, "});
}

TEST_F(AlgoTest, for_each_n) {
  std::vector<int> ints{0, 1, 2, 3, 4};
  // needs bond check.
  std::for_each_n(ints.begin(), 5, [](int) -> void {
    // no-op;
  });
}

TEST_F(AlgoTest, any_of) {
  std::vector<int> ints{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto result = std::any_of(ints.cbegin(), ints.cend(),
                            [](int value) -> bool { return value == 5; });

  ASSERT_TRUE(result);
}

TEST_F(AlgoTest, all_of) {
  std::vector<int> ints{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto result = std::all_of(ints.cbegin(), ints.cend(),
                            [](int value) -> bool { return value != 5; });

  ASSERT_FALSE(result);
}

TEST_F(AlgoTest, none_of) {
  std::vector<int> ints{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto result = std::none_of(ints.cbegin(), ints.cend(),
                             [](int value) -> bool { return value > 100; });

  ASSERT_TRUE(result);
}

TEST_F(AlgoTest, find) {
  std::vector<int> ints{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto iter = std::find(ints.cbegin(), ints.cend(), 5);

  ASSERT_NE(iter, ints.end());
  ASSERT_EQ(*iter, 5);
}

TEST_F(AlgoTest, find_if) {
  std::vector<int> ints{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  {
    auto iter = std::find_if(ints.cbegin(), ints.cend(),
                             [](int value) -> bool { return value == 5; });
    ASSERT_NE(iter, ints.end());
    ASSERT_EQ(*iter, 5);
  }

  {
    auto iter = std::find_if(ints.cbegin(), ints.cend(),
                             [](int value) -> bool { return value == 124; });
    ASSERT_EQ(iter, ints.end());
  }
}

TEST_F(AlgoTest, find_if_not) {
  std::vector<int> ints{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  {
    auto iter = std::find_if_not(ints.cbegin(), ints.cend(),
                                 [](int value) -> bool { return value == 0; });
    ASSERT_NE(iter, ints.end());
    ASSERT_EQ(*iter, 1);
  }

  {
    auto iter = std::find_if(ints.cbegin(), ints.cend(),
                             [](int value) -> bool { return value == 124; });
    ASSERT_EQ(iter, ints.end());
  }
}

TEST_F(AlgoTest, find_first_of) {
  std::vector<int> ints{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<int> sub_ints{4, 5};
  auto iter = std::find_first_of(ints.cbegin(), ints.cend(), sub_ints.cbegin(),
                                 sub_ints.cend());

  ASSERT_EQ(*iter, 4);
}

TEST_F(AlgoTest, find_end) {
  std::vector<int> ints{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<int> sub_ints{4, 5};
  auto iter = std::find_end(ints.cbegin(), ints.cend(), sub_ints.cbegin(),
                            sub_ints.cend());

  ASSERT_EQ(*iter, 4);
  ASSERT_NE(iter, ints.end());
}

TEST_F(AlgoTest, search) {
  std::string string = "Hello world";
  std::string target = "world";

  auto iter = std::search(string.cbegin(), string.cend(), target.cbegin(),
                          target.cend());
  ASSERT_EQ(*iter, 'w');
}

TEST_F(AlgoTest, search_n) {
  std::string string = "___AAA___";

  {
    auto iter = std::search_n(string.cbegin(), string.cend(), 4,
                              'A');  // search the sequence of AAAA
    ASSERT_EQ(iter, string.cend());
  }

  {
    auto iter = std::search_n(string.cbegin(), string.cend(), 3,
                              'A');  // search the sequence of AAA
    ASSERT_NE(iter, string.cend());
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::AddGlobalTestEnvironment(new AlgoTestEnviroment{});
  return RUN_ALL_TESTS();
}