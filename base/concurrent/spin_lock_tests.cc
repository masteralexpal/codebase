/**
 * @file spin_lock_tests.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include "base/concurrent/spin_lock.h"
#include <gtest/gtest.h>

inline constexpr int kNumOfThreads = 10;
inline constexpr int kMax = 100000;

TEST(SpinLock, Counting) {

  int count = 0;

  base::SpinLock spin_lock;
  {
    std::vector<std::thread> threads;
    for (int i = 0; i < kNumOfThreads; ++i) {
      threads.emplace_back([&]() mutable -> void {
        for (int j = 0; j < kMax; ++j) {
          spin_lock.Acquire();
          ++count;
          spin_lock.Release();
        }
      });
    }
    for (auto &&thread : threads) {
      thread.join();
    }
  }
  ASSERT_EQ(count, kMax * kNumOfThreads);
}

TEST(StdMutex, Counting) {

  int count = 0;

  std::mutex mutex;
  {
    std::vector<std::thread> threads;
    for (int i = 0; i < kNumOfThreads; ++i) {
      threads.emplace_back([&]() mutable -> void {
        for (int j = 0; j < kMax; ++j) {
          mutex.lock();
          ++count;
          mutex.unlock();
        }
      });
    }
    for (auto &&thread : threads) {
      thread.join();
    }
  }
  ASSERT_EQ(count, kMax * kNumOfThreads);
}

TEST(Atomic, Counting) {

  std::atomic<int> count = 0;

  {
    std::vector<std::thread> threads;
    for (int i = 0; i < kNumOfThreads; ++i) {
      threads.emplace_back([&]() mutable -> void {
        for (int j = 0; j < kMax; ++j) {
          ++count;
        }
      });
    }
    for (auto &&thread : threads) {
      thread.join();
    }
  }

  ASSERT_EQ(count, kMax * kNumOfThreads);
}