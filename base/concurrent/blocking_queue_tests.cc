/**
 * @file blocking_queue_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <gtest/gtest.h>
#include "base/concurrent/blocking_queue.h"
#include "base/concurrent/thread.h"

namespace base {
TEST(BlockingQueue, case0) {
  BlockingQueue<int> blocking_queue;
  ASSERT_EQ(blocking_queue.Size(), 0u);
}
TEST(BlockingQueue, case1) {
  BlockingQueue<int> blocking_queue;
  std::size_t limit = 100;
  auto producer = [&blocking_queue, &limit]() -> void {
    for (auto i = 0ull; i < limit; ++i) {
      blocking_queue.Enqueue(i);
      std::cout << "Enqueue: i = " << i << ", Size = " << blocking_queue.Size()
                << std::endl;
      // base::CurrentThread::SleepFor(std::chrono::seconds{1});
    }
  };

  auto consumer = [&blocking_queue, &limit]() -> void {
    for (auto i = 0ull; i < limit; ++i) {
      int n = blocking_queue.Dequeue();
      std::cout << "Dequque: n = " << n << ", Size = " << blocking_queue.Size()
                << std::endl;
      // base::CurrentThread::SleepFor(std::chrono::seconds{1});
    }
  };

  base::Thread thread1;
  base::Thread thread2;
  thread1.Start(producer);
  thread2.Start(consumer);
  thread1.Join();
  thread2.Join();
}
}  // namespace base