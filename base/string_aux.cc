/**
 * @file string_auxiliary.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/string_aux.h"
#include <cctype>
#include <string>
#include <vector>

namespace base {
std::vector<std::string> Split(std::string const& text, char delimiter) {
  std::vector<std::string> strings;
  auto slow = text.begin();
  auto fast = text.begin();
  while (fast != text.end()) {
    if (*fast == delimiter) {
      strings.emplace_back(slow, fast);
      slow = ++fast;
    } else {
      ++fast;
    }
  }
  strings.emplace_back(slow, fast);
  return strings;
}

std::vector<std::string> Split(std::string const& text,
                               char delimiter,
                               int degree) {
  std::vector<std::string> strings;
  auto slow = text.begin();
  auto fast = text.begin();
  while (fast != text.end()) {
    if (*fast == delimiter) {
      if (--degree < 0)
        break;
      strings.emplace_back(slow, fast);
      slow = ++fast;
    } else {
      ++fast;
    }
  }
  strings.emplace_back(slow, text.end());
  return strings;
}

std::vector<std::string> ReverseSplit(std::string const& text, char delimiter) {
  std::vector<std::string> strings;
  auto fast = text.end();
  auto slow = text.end();
  while (fast != text.begin()) {
    --fast;
    if (*fast == delimiter) {
      strings.emplace_back(fast + 1, slow);
      slow = fast;
    }
  }
  strings.emplace_back(fast, slow);
  return strings;
}

std::vector<std::string> ReverseSplit(std::string const& text,
                                      char delimiter,
                                      int degree) {
  std::vector<std::string> strings;
  auto fast = text.end();
  auto slow = text.end();
  while (fast != text.begin()) {
    --fast;
    if (*fast == delimiter) {
      if (--degree < 0)
        break;
      strings.emplace_back(fast + 1, slow);
      slow = fast;
    }
  }
  strings.emplace_back(text.begin(), slow);
  return strings;
}

void LeftTrim(std::string& text) {
  auto iter = text.begin();
  while (iter != text.end() && std::isspace(*iter)) {
    ++iter;
  }
  text.erase(text.begin(), iter);
}

void RightTrim(std::string& text) {
  auto iter = text.rbegin();
  while (iter != text.rend() && std::isspace(*iter)) {
    ++iter;
  }
  text.erase(iter.base(), text.rbegin().base());
}

void Trim(std::string& text) {
  LeftTrim(text);
  RightTrim(text);
}

std::string LeftTrim(std::string const& text) {
  auto iter = text.begin();
  while (iter != text.end() && std::isspace(*iter)) {
    ++iter;
  }
  return std::string{iter, text.end()};
}

std::string RightTrim(std::string const& text) {
  auto iter = text.rbegin();
  while (iter != text.rend() && std::isspace(*iter)) {
    ++iter;
  }
  return std::string{text.begin(), iter.base()};
}

std::string Trim(std::string const& text) {
  auto left = text.begin();
  while (left != text.end() && std::isspace(*left)) {
    ++left;
  }
  auto right = text.rbegin();
  while (right != text.rend() && std::isspace(*right)) {
    ++right;
  }
  return std::string{left, right.base()};
}

bool StartsWith(std::string const& text, std::string const& prefix) {
  if (text.size() < prefix.size() || text.empty() || prefix.empty()) {
    return false;
  }
  auto text_iter = text.begin();
  auto perfix_iter = prefix.begin();
  while (perfix_iter != prefix.end()) {
    if (*perfix_iter != *text_iter) {
      return false;
    }
    perfix_iter++;
    text_iter++;
  }
  return true;
}

bool EndsWith(std::string const& text, std::string const& suffix) {
  if (text.size() < suffix.size() || text.empty() || suffix.empty()) {
    return false;
  }
  auto text_iter = text.rbegin();
  auto suffix_iter = suffix.rbegin();
  while (suffix_iter != suffix.rend()) {
    if (*suffix_iter != *text_iter) {
      return false;
    }
    ++suffix_iter;
    ++text_iter;
  }
  return true;
}

void SetBit(size_t& bits, size_t index) {
  bits |= (static_cast<size_t>(1) << index);
}

void ResetBit(size_t& bits, size_t index) {
  bits &= ~(static_cast<size_t>(1) << index);
}

void FlipBit(size_t& bits, size_t index) {
  bits ^= (static_cast<size_t>(1) << index);
}
}  // namespace base