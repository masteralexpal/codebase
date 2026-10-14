/**
 * @file count_down_latch_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/concurrent/count_down_latch.h"

#include <chrono>
#include <thread>

#include <gtest/gtest.h>

namespace base {

TEST(CountdownLatch, Constructor) {
  CountdownLatch count_down_latch{8};
  ASSERT_EQ(count_down_latch.GetCount(), 8);
}

TEST(CountdownLatch, Wait) {
  CountdownLatch count_down_latch{2};
  int value = 2;
  std::thread thread0([&]() mutable -> void {
    count_down_latch.Wait();
    --value;
  });
  std::thread thread1([&]() mutable -> void {
    count_down_latch.Wait();
    --value;
  });
  std::this_thread::sleep_for(std::chrono::seconds{1});
  ASSERT_EQ(value, 2);
  ASSERT_EQ(count_down_latch.GetCount(), 2);
  count_down_latch.CountDown();
  ASSERT_EQ(count_down_latch.GetCount(), 1);
  count_down_latch.CountDown();
  ASSERT_EQ(count_down_latch.GetCount(), 0);
  std::this_thread::sleep_for(std::chrono::seconds{1});
  ASSERT_EQ(value, 0);
  thread0.join();
  thread1.join();
}

}  // namespace base