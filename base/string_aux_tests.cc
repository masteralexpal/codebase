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
  std::vector<std::string> const& result = Split("www.example.com", '.');
  std::vector<std::string> expect{"www", "example", "com"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_1) {
  std::vector<std::string> const& result = Split(".www.example.com.", '.');
  std::vector<std::string> expect{"", "www", "example", "com", ""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_2) {
  std::vector<std::string> const& result = Split("...", '.');
  std::vector<std::string> expect{"", "", "", ""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_3) {
  std::vector<std::string> const& result = Split("", '.');
  std::vector<std::string> expect{""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_4) {
  std::vector<std::string> const& result = Split("www.example.com", '.', 0);
  std::vector<std::string> expect{"www.example.com"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_5) {
  std::vector<std::string> const& result = Split("www.example.com", '.', 1);
  std::vector<std::string> expect{"www", "example.com"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_6) {
  std::vector<std::string> const& result = Split("www.example.com", '.', 9);
  std::vector<std::string> expect{"www", "example", "com"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, split_7) {
  std::vector<std::string> const& result = Split("www.example.com", '.', -1);
  std::vector<std::string> expect{"www.example.com"};
  ASSERT_EQ(result, expect);
}

// testing rsplit

TEST(auxiliary, rsplit_0) {
  std::vector<std::string> const& result = ReverseSplit("www.example.com", '.');
  std::vector<std::string> expect{"com", "example", "www"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_1) {
  std::vector<std::string> const& result =
      ReverseSplit(".www.example.com.", '.');
  std::vector<std::string> expect{"", "com", "example", "www", ""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_2) {
  std::vector<std::string> const& result = ReverseSplit("...", '.');
  std::vector<std::string> expect{"", "", "", ""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_3) {
  std::vector<std::string> const& result = ReverseSplit("", '.');
  std::vector<std::string> expect{""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_4) {
  std::vector<std::string> const& result =
      ReverseSplit("www.example.com", '.', 0);
  std::vector<std::string> expect{"www.example.com"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_5) {
  std::vector<std::string> const& result =
      ReverseSplit("www.example.com", '.', 1);
  std::vector<std::string> expect{"com", "www.example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_6) {
  std::vector<std::string> const& result =
      ReverseSplit("www.example.com", '.', 9);
  std::vector<std::string> expect{"com", "example", "www"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rsplit_7) {
  std::vector<std::string> const& result =
      ReverseSplit("www.example.com", '.', -1);
  std::vector<std::string> expect{"www.example.com"};
  ASSERT_EQ(result, expect);
}
// testing ltrim
TEST(auxiliary, ltrim_0) {
  std::string const& result = LeftTrim("    example");
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_1) {
  std::string const& result = LeftTrim("example    ");
  std::string const expect{"example    "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_2) {
  std::string const& result = LeftTrim("    ");
  std::string const expect;
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_3) {
  std::string const& result = LeftTrim("example");
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_4) {
  std::string const& result = LeftTrim("     example     ");
  std::string const expect{"example     "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_5) {
  std::string const& result = LeftTrim("     exa m ple     ");
  std::string const expect{"exa m ple     "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_6) {
  std::string const& result = LeftTrim("  exa m ple  ");
  std::string const expect{"exa m ple  "};
  ASSERT_EQ(result, expect);
}

// alternative form
TEST(auxiliary, ltrim_7) {
  std::string result = "    example";
  LeftTrim(result);
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_8) {
  std::string result = "example    ";
  LeftTrim(result);
  std::string const expect{"example    "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_9) {
  std::string result = "    ";
  LeftTrim(result);
  std::string const expect;
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_10) {
  std::string result = "example";
  LeftTrim(result);
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_11) {
  std::string result = "     example     ";
  LeftTrim(result);
  std::string const expect{"example     "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_12) {
  std::string result = "     exa m ple     ";
  LeftTrim(result);
  std::string const expect{"exa m ple     "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, ltrim_13) {
  std::string result = "  exa m ple  ";
  LeftTrim(result);
  std::string const expect{"exa m ple  "};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_0) {
  std::string const& result = RightTrim("    example");
  std::string const expect = "    example";
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_1) {
  std::string const& result = RightTrim("example    ");
  std::string const expect = "example";
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_2) {
  std::string const& result = RightTrim("    example    ");
  std::string const expect = "    example";
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_3) {
  std::string const& result = RightTrim("example");
  std::string const expect = "example";
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_4) {
  std::string const& result = RightTrim("    ");
  std::string const expect;
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_5) {
  std::string const& result = RightTrim("     exa m ple     ");
  std::string const expect = "     exa m ple";
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_6) {
  std::string const& result = RightTrim("  exa m ple  ");
  std::string const expect = "  exa m ple";
  ASSERT_EQ(result, expect);
}

// alternative form
TEST(auxiliary, rtrim_7) {
  std::string result = "    example";
  RightTrim(result);
  std::string const expect{"    example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_8) {
  std::string result = "example    ";
  RightTrim(result);
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_9) {
  std::string result = "    ";
  RightTrim(result);
  std::string const expect;
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_10) {
  std::string result = "example";
  RightTrim(result);
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_11) {
  std::string result = "     example     ";
  RightTrim(result);
  std::string const expect{"     example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_12) {
  std::string result = "     exa m ple     ";
  RightTrim(result);
  std::string const expect{"     exa m ple"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, rtrim_13) {
  std::string result = "  exa m ple  ";
  RightTrim(result);
  std::string const expect{"  exa m ple"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_0) {
  std::string const& result = Trim("    example");
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_1) {
  std::string const& result = Trim("example     ");
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_2) {
  std::string const& result = Trim("");
  std::string const expect{""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_3) {
  std::string const& result = Trim("example");
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_4) {
  std::string const& result = Trim("     example     ");
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_5) {
  std::string const& result = Trim("     e x a m p l e     ");
  std::string const expect{"e x a m p l e"};
  ASSERT_EQ(result, expect);
}
// alternative form
TEST(auxiliary, trim_6) {
  std::string result = {"example     "};
  Trim(result);
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_7) {
  std::string result = {"     example"};
  Trim(result);
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_8) {
  std::string result = {"example"};
  Trim(result);
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_9) {
  std::string result = {"     example     "};
  Trim(result);
  std::string const expect{"example"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_10) {
  std::string result = {"     e x a m p l e     "};
  Trim(result);
  std::string const expect{"e x a m p l e"};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, trim_11) {
  std::string result = {""};
  Trim(result);
  std::string const expect{""};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, startswith_0) {
  std::string const text{"example"};
  ASSERT_TRUE(StartsWith(text, "e"));
}

TEST(auxiliary, startswith_1) {
  std::string const text{"example"};
  ASSERT_TRUE(StartsWith(text, "example"));
}

TEST(auxiliary, startswith_2) {
  std::string const text{"example"};
  ASSERT_FALSE(StartsWith(text, ""));
}

TEST(auxiliary, startswith_3) {
  std::string const text{""};
  ASSERT_FALSE(StartsWith(text, ""));
}

TEST(auxiliary, startswith_4) {
  std::string const text{"   example"};
  ASSERT_TRUE(StartsWith(text, " "));
}

TEST(auxiliary, startswith_5) {
  std::string const text{" "};
  ASSERT_TRUE(StartsWith(text, " "));
}

TEST(auxiliary, endswith_0) {
  std::string const text{"example"};
  ASSERT_TRUE(EndsWith(text, "e"));
}

TEST(auxiliary, endswith_1) {
  std::string const text{"example"};
  ASSERT_TRUE(EndsWith(text, "example"));
}

TEST(auxiliary, endswith_2) {
  std::string const text{"example"};
  ASSERT_FALSE(EndsWith(text, ""));
}

TEST(auxiliary, endswith_3) {
  std::string const text{""};
  ASSERT_FALSE(EndsWith(text, ""));
}

TEST(auxiliary, endswith_4) {
  std::string const text{"example   "};
  ASSERT_TRUE(EndsWith(text, " "));
}

TEST(auxiliary, endswith_5) {
  std::string const text{" "};
  ASSERT_TRUE(EndsWith(text, " "));
}

TEST(auxiliary, setBit_0) {
  size_t result{0};
  SetBit(result, 0);
  SetBit(result, 1);
  size_t const expect{3};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, setBit_1) {
  size_t result{0};
  SetBit(result, 1);
  SetBit(result, 1);
  size_t const expect{2};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, resetBit_0) {
  size_t result{3};
  ResetBit(result, 0);
  ResetBit(result, 1);
  size_t const expect{0};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, resetBit_1) {
  size_t result{3};
  ResetBit(result, 1);
  ResetBit(result, 1);
  size_t const expect{1};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, flipBit_0) {
  size_t result{1};
  FlipBit(result, 1);
  FlipBit(result, 1);
  size_t const expect{1};
  ASSERT_EQ(result, expect);
}

TEST(auxiliary, flipBit_1) {
  size_t result{1};
  FlipBit(result, 1);
  FlipBit(result, 1);
  FlipBit(result, 1);
  size_t const expect{3};
  ASSERT_EQ(result, expect);
}
}  // namespace base