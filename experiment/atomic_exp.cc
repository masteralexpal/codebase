/**
 * @file atomic.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <atomic>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

static constexpr int kMax = 1'000'000;
static constexpr int kThreadCount = 40;

int volatile value = 0;
std::atomic_int atomic_value = 0;

void Accumulate() {
  for (int i = 0; i < kMax; ++i) {
    value = value + 1;
  }
}

TEST(Volatile, Accumulate) {
  std::vector<std::thread> worker_threads;
  for (int i = 0; i < kThreadCount; ++i) {
    worker_threads.emplace_back(Accumulate);
  }

  for (auto &&thread : worker_threads) {
    thread.join();
  }

  ASSERT_LT(value, kThreadCount * kMax);
}

void AtomicAccumulate() {
  for (int i = 0; i < kMax; ++i) {
    atomic_value.fetch_add(1, std::memory_order_relaxed);
  }
}

TEST(Atomic, Accumulate) {
  std::vector<std::thread> worker_threads;
  for (int i = 0; i < kThreadCount; ++i) {
    worker_threads.emplace_back(AtomicAccumulate);
  }

  for (auto &&thread : worker_threads) {
    thread.join();
  }
  ASSERT_EQ(atomic_value, kThreadCount * kMax);
}