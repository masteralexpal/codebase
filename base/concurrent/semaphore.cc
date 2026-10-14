/**
 * @file semaphore.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "base/concurrent/semaphore.h"

namespace base {
Semaphore::Semaphore() : count_(0) {}
Semaphore::Semaphore(int count) : count_(count) {}

Semaphore::~Semaphore() = default;

void Semaphore::Wait() {
  mtx_.Acquire();
  while (count_ <= 0) {
    cv_.Wait(mtx_);
  }
  --count_;
  mtx_.Release();
};

void Semaphore::Signal() {
  mtx_.Acquire();
  ++count_;
  mtx_.Release();
  cv_.Signal();
};

}  // namespace base