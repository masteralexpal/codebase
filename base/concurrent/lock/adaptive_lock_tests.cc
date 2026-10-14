/**
 * @file adaptive_lock_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-07-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#include <gtest/gtest.h>
#include "base/concurrent/lock/adaptive_lock.h"

class AdaptiveLockTests : public testing::Test {
 public:
  AdaptiveLockTests() = default;

  void SetUp() override {}

  void TestTryLock() {
    if (lock.TryLock()) {
      ++counter;
      lock.UnLock();
    } else {
      failded_try_lock.fetch_add(1, std::memory_order_acq_rel);
    }
  }

  void TestLock() {
    lock.Lock();
    ++counter;
    lock.UnLock();
  }

  void TearDown() override {
    std::cout << "counter = " << counter << std::endl;
    std::cout << "failded_try_lock = " << failded_try_lock << std::endl;
    std::cout << "failded_try_lock + failded_try_lock = "
              << counter + failded_try_lock << std::endl;
  }

  int counter = 0;
  base::AdaptiveLock lock;
  std::atomic<int> failded_try_lock = 0;
  int thread_num = 19;
  int loop_count = 1'000'000;
};

TEST_F(AdaptiveLockTests, Lock) {
  std::vector<std::thread> threads;
  for (auto i = 0; i < this->thread_num; ++i) {
    threads.emplace_back([self = this]() -> void {
      for (auto j = 0; j < self->loop_count; ++j) {
        self->TestLock();
      }
    });
  }

  for (auto&& thread : threads) {
    thread.join();
  }

  ASSERT_EQ(counter, thread_num * loop_count);
}

TEST_F(AdaptiveLockTests, TryLock) {
  std::vector<std::thread> threads;
  for (auto i = 0; i < this->thread_num; ++i) {
    threads.emplace_back([self = this]() -> void {
      for (auto j = 0; j < self->loop_count; ++j) {
        self->TestTryLock();
      }
    });
  }

  for (auto&& thread : threads) {
    thread.join();
  }

  ASSERT_EQ(failded_try_lock + counter, thread_num * loop_count);
}
