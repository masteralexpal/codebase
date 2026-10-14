/**
 * @file semaphore.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONCURRENT_SEMAPHORE_H_
#define BASE_CONCURRENT_SEMAPHORE_H_

#include "base/base_exports.h"
#include "base/concurrent/condition_variable.h"
#include "base/concurrent/mutex.h"

namespace base {
class BASE_EXPORT Semaphore final {
 public:
  Semaphore();
  Semaphore(int count);

  Semaphore(Semaphore const&) = delete;
  Semaphore& operator=(Semaphore const&) = delete;

  ~Semaphore();

  void Wait();

  void Signal();

 private:
  int count_;
  Mutex mtx_;
  ConditionVariable cv_;
};
}  // namespace base
#endif