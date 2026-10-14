#include <gtest/gtest.h>
#include "base/concurrent/mutex.h"
#include "base/concurrent/mutex_guard.h"
#include "base/concurrent/restrict_mutex.h"
#include "base/concurrent/thread.h"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <thread>
#include <vector>

namespace base {
TEST(Mutex, Accumulation) {
  int count = 0u;
  int thread_count = 10;
  int accumulation_per_thread = 1'000'000;
  auto l = [&]() mutable -> void {
    for (int i = 0; i < accumulation_per_thread; ++i) {
      ++count;
    }
  };

  for (int i = 0; i < thread_count; ++i) {
    Thread thread;
    thread.Start(l);
  }
  // Bad.
  std::this_thread::sleep_for(std::chrono::seconds{5});
  ASSERT_NE(count, thread_count * accumulation_per_thread);
}

TEST(Mutex, AccumulationWithMutex) {
  Mutex mutex;
  int count = 0u;
  int thread_count = 10;
  int accumulation_per_thread = 1'000'000;
  auto l = [&]() mutable -> void {
    mutex.Acquire();
    for (int i = 0; i < accumulation_per_thread; ++i) {
      ++count;
    }
    mutex.Release();
  };

  for (int i = 0; i < thread_count; ++i) {
    Thread thread;
    thread.Start(l);
  }
  // Bad.
  std::this_thread::sleep_for(std::chrono::seconds{5});
  ASSERT_EQ(count, thread_count * accumulation_per_thread);
}

TEST(Mutex, MutexGuard) {
  base::Mutex mutex;
  int count = 0u;
  int thread_count = 10;
  int accumulation_per_thread = 1'000'000;
  auto l = [&]() mutable -> void {
    MutexGuard<base::Mutex> lock_guard{mutex};
    for (int i = 0; i < accumulation_per_thread; ++i) {
      ++count;
    }
  };

  for (int i = 0; i < thread_count; ++i) {
    Thread thread;
    thread.Start(l);
  }
  // Bad.
  std::this_thread::sleep_for(std::chrono::seconds{5});
  ASSERT_EQ(count, thread_count * accumulation_per_thread);
}
}  // namespace base
