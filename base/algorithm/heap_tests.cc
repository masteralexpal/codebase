/**
 * @file heap_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/algorithm/heap.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <vector>

TEST(Heap, MaxHeap) {
  base::MaxHeap<int> heck{10};
  std::vector<int> vector = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(vector.begin(), vector.end(), g);
  for (auto &&i : vector) {
    std::cout << i;
  }
  std::cout << std::endl;
  for (int i = 0; i < 10; ++i) {
    heck.Push(vector[i]);
  }
  while (!heck.Empty()) {
    std::cout << heck.Pop();
  }
}

TEST(Heap, MinHeap) {
  base::MinHeap<int> heck{10};
  std::vector<int> vector = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(vector.begin(), vector.end(), g);
  for (auto &&i : vector) {
    std::cout << i;
  }
  std::cout << std::endl;
  for (int i = 0; i < 10; ++i) {
    heck.Push(vector[i]);
  }
  while (!heck.Empty()) {
    std::cout << heck.Pop();
  }
}