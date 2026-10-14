/**
 * @file string_auxiliary_tests.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/string_aux.h"

#include <gtest/gtest.h>

namespace base {
TEST(auxiliary, split_0) {
  const std::vector<std::string> &result = Split("www.example.com", '.');
  std::vector<std::string> expect{"www", "example", "com"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_1) {
  const std::vector<std::string> &result = Split(".www.example.com.", '.');
  std::vector<std::string> expect{"", "www", "example", "com", ""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_2) {
  const std::vector<std::string> &result = Split("...", '.');
  std::vector<std::string> expect{"", "", "", ""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_3) {
  const std::vector<std::string> &result = Split("", '.');
  std::vector<std::string> expect{""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_4) {
  const std::vector<std::string> &result = Split("www.example.com", '.', 0);
  std::vector<std::string> expect{"www.example.com"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_5) {
  const std::vector<std::string> &result = Split("www.example.com", '.', 1);
  std::vector<std::string> expect{"www", "example.com"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_6) {
  const std::vector<std::string> &result = Split("www.example.com", '.', 9);
  std::vector<std::string> expect{"www", "example", "com"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_7) {
  const std::vector<std::string> &result = Split("www.example.com", '.', -1);
  std::vector<std::string> expect{"www.example.com"};
  ASSERT_EQ(result, expect);
}

// testing rsplit

TEST(auxiliary, rsplit_0) {
  const std::vector<std::string> &result = ReverseSplit("www.example.com", '.');
  std::vector<std::string> expect{"com", "example", "www"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_1) {
  const std::vector<std::string> &result =
      ReverseSplit(".www.example.com.", '.');
  std::vector<std::string> expect{"", "com", "example", "www", ""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_2) {
  const std::vector<std::string> &result = ReverseSplit("...", '.');
  std::vector<std::string> expect{"", "", "", ""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_3) {
  const std::vector<std::string> &result = ReverseSplit("", '.');
  std::vector<std::string> expect{""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_4) {
  const std::vector<std::string> &result =
      ReverseSplit("www.example.com", '.', 0);
  std::vector<std::string> expect{"www.example.com"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_5) {
  const std::vector<std::string> &result =
      ReverseSplit("www.example.com", '.', 1);
  std::vector<std::string> expect{"com", "www.example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_6) {
  const std::vector<std::string> &result =
      ReverseSplit("www.example.com", '.', 9);
  std::vector<std::string> expect{"com", "example", "www"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_7) {
  const std::vector<std::string> &result =
      ReverseSplit("www.example.com", '.', -1);
  std::vector<std::string> expect{"www.example.com"};
  ASSERT_EQ(result, expect);
}
// testing ltrim
TEST(auxiliary, ltrim_0) {
  const std::string &result = LeftTrim("    example");
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_1) {
  const std::string &result = LeftTrim("example    ");
  const std::string expect{"example    "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_2) {
  const std::string &result = LeftTrim("    ");
  const std::string expect;
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_3) {
  const std::string &result = LeftTrim("example");
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_4) {
  const std::string &result = LeftTrim("     example     ");
  const std::string expect{"example     "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_5) {
  const std::string &result = LeftTrim("     exa m ple     ");
  const std::string expect{"exa m ple     "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_6) {
  const std::string &result = LeftTrim("  exa m ple  ");
  const std::string expect{"exa m ple  "};
  ASSERT_EQ(result, expect);
}

// alternative form
TEST(auxiliary, ltrim_7) {
  std::string result = "    example";
  LeftTrim(result);
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_8) {
  std::string result = "example    ";
  LeftTrim(result);
  const std::string expect{"example    "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_9) {
  std::string result = "    ";
  LeftTrim(result);
  const std::string expect;
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_10) {

  std::string result = "example";
  LeftTrim(result);
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_11) {
  std::string result = "     example     ";
  LeftTrim(result);
  const std::string expect{"example     "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_12) {

  std::string result = "     exa m ple     ";
  LeftTrim(result);
  const std::string expect{"exa m ple     "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_13) {
  std::string result = "  exa m ple  ";
  LeftTrim(result);
  const std::string expect{"exa m ple  "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_0) {
  const std::string &result = RightTrim("    example");
  const std::string expect = "    example";
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_1) {
  const std::string &result = RightTrim("example    ");
  const std::string expect = "example";
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_2) {
  const std::string &result = RightTrim("    example    ");
  const std::string expect = "    example";
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_3) {
  const std::string &result = RightTrim("example");
  const std::string expect = "example";
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_4) {
  const std::string &result = RightTrim("    ");
  const std::string expect;
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_5) {
  const std::string &result = RightTrim("     exa m ple     ");
  const std::string expect = "     exa m ple";
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_6) {
  const std::string &result = RightTrim("  exa m ple  ");
  const std::string expect = "  exa m ple";
  ASSERT_EQ(result, expect);
}

// alternative form
TEST(auxiliary, rtrim_7) {
  std::string result = "    example";
  RightTrim(result);
  const std::string expect{"    example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_8) {
  std::string result = "example    ";
  RightTrim(result);
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_9) {
  std::string result = "    ";
  RightTrim(result);
  const std::string expect;
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_10) {

  std::string result = "example";
  RightTrim(result);
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_11) {
  std::string result = "     example     ";
  RightTrim(result);
  const std::string expect{"     example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_12) {

  std::string result = "     exa m ple     ";
  RightTrim(result);
  const std::string expect{"     exa m ple"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_13) {
  std::string result = "  exa m ple  ";
  RightTrim(result);
  const std::string expect{"  exa m ple"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_0) {
  const std::string &result = Trim("    example");
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_1) {
  const std::string &result = Trim("example     ");
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_2) {
  const std::string &result = Trim("");
  const std::string expect{""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_3) {
  const std::string &result = Trim("example");
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_4) {
  const std::string &result = Trim("     example     ");
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_5) {
  const std::string &result = Trim("     e x a m p l e     ");
  const std::string expect{"e x a m p l e"};
  ASSERT_EQ(result, expect);
}
// alternative form
TEST(auxiliary, trim_6) {
  std::string result = {"example     "};
  Trim(result);
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_7) {
  std::string result = {"     example"};
  Trim(result);
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_8) {
  std::string result = {"example"};
  Trim(result);
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_9) {
  std::string result = {"     example     "};
  Trim(result);
  const std::string expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_10) {
  std::string result = {"     e x a m p l e     "};
  Trim(result);
  const std::string expect{"e x a m p l e"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_11) {
  std::string result = {""};
  Trim(result);
  const std::string expect{""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, startswith_0) {
  const std::string text{"example"};
  ASSERT_TRUE(StartsWith(text, "e"));
}

TEST(auxiliary, startswith_1) {
  const std::string text{"example"};
  ASSERT_TRUE(StartsWith(text, "example"));
}

TEST(auxiliary, startswith_2) {
  const std::string text{"example"};
  ASSERT_FALSE(StartsWith(text, ""));
}

TEST(auxiliary, startswith_3) {
  const std::string text{""};
  ASSERT_FALSE(StartsWith(text, ""));
}

TEST(auxiliary, startswith_4) {
  const std::string text{"   example"};
  ASSERT_TRUE(StartsWith(text, " "));
}

TEST(auxiliary, startswith_5) {
  const std::string text{" "};
  ASSERT_TRUE(StartsWith(text, " "));
}

TEST(auxiliary, endswith_0) {
  const std::string text{"example"};
  ASSERT_TRUE(EndsWith(text, "e"));
}

TEST(auxiliary, endswith_1) {
  const std::string text{"example"};
  ASSERT_TRUE(EndsWith(text, "example"));
}

TEST(auxiliary, endswith_2) {
  const std::string text{"example"};
  ASSERT_FALSE(EndsWith(text, ""));
}

TEST(auxiliary, endswith_3) {
  const std::string text{""};
  ASSERT_FALSE(EndsWith(text, ""));
}

TEST(auxiliary, endswith_4) {
  const std::string text{"example   "};
  ASSERT_TRUE(EndsWith(text, " "));
}

TEST(auxiliary, endswith_5) {
  const std::string text{" "};
  ASSERT_TRUE(EndsWith(text, " "));
}

TEST(auxiliary, setBit_0) {
  size_t result{0};
  SetBit(result, 0);
  SetBit(result, 1);
  const size_t expect{3};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, setBit_1) {
  size_t result{0};
  SetBit(result, 1);
  SetBit(result, 1);
  const size_t expect{2};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, resetBit_0) {
  size_t result{3};
  ResetBit(result, 0);
  ResetBit(result, 1);
  const size_t expect{0};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, resetBit_1) {
  size_t result{3};
  ResetBit(result, 1);
  ResetBit(result, 1);
  const size_t expect{1};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, flipBit_0) {
  size_t result{1};
  FlipBit(result, 1);
  FlipBit(result, 1);
  const size_t expect{1};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, flipBit_1) {
  size_t result{1};
  FlipBit(result, 1);
  FlipBit(result, 1);
  FlipBit(result, 1);
  const size_t expect{3};
  ASSERT_EQ(result, expect);
}
} // namespace base