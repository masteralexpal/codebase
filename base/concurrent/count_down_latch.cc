/**
 * @file count_down_latch.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/concurrent/count_down_latch.h"

#include "base/concurrent/mutex_guard.h"

namespace base {
CountdownLatch::CountdownLatch(int const& count)
    : count_(count), mtx_(), cv_() {}

void CountdownLatch::CountDown() {
  {
    MutexGuard<Mutex> mutex_guard{mtx_};
    --count_;
  }
  if (count_ == 0) {
    cv_.Broadcast();
  }
}

void CountdownLatch::Wait() {
  mtx_.Acquire();
  while (count_ != 0) {
    cv_.Wait(mtx_);
  }
  mtx_.Release();
}

int CountdownLatch::GetCount() {
  return count_;
}
}  // namespace base