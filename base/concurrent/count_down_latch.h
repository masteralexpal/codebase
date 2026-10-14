/**
 * @file count_down_latch.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONCURRENT_COUNT_DOWN_LATCH_H_
#define BASE_CONCURRENT_COUNT_DOWN_LATCH_H_

#include "base/concurrent/condition_variable.h"
#include "base/concurrent/mutex.h"

namespace base {
class CountdownLatch {
 public:
  CountdownLatch(int const& count);

  void CountDown();

  void Wait();

  int GetCount();

 private:
  int count_;
  Mutex mtx_;
  ConditionVariable cv_;
};
}  // namespace base
#endif  //  BASE_CONCURRENT_COUNT_DOWN_LATCH_H_