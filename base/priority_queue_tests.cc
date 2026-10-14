/**
 * @file priority_queue_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <gtest/gtest.h>
#include "base/priority_queue.h"

namespace base {
TEST(PriorityQueue, case0) {
  PriorityQueue<int> priority_queue;
  std::vector<int> vector = {12, 22, 34, 14, 45, 62, 17, 84, 96};
  for (auto&& i : vector) {
    priority_queue.Insert(i);
  }
  while (!priority_queue.Empty()) {
    std::cout << priority_queue.Max() << ", ";
    priority_queue.PopMax();
  }
  std::cout << std::endl;
  ASSERT_EQ(priority_queue.Empty(), true);
}

TEST(MaxHeap, case0) {
  MaxHeap<int> max_heap(10);
  std::vector<int> vector = {12, 22, 34, 14, 45, 62, 17, 84, 96};
  for (auto&& i : vector) {
    max_heap.Insert(i);
  }
  while (!max_heap.Empty()) {
    std::cout << max_heap.PopMax() << ", ";
  }
  std::cout << std::endl;
}
}  // namespace base