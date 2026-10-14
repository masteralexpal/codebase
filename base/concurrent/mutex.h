/**
 * @file mutex.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-07-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef BASE_CONCURRENT_MUTEX_H_
#define BASE_CONCURRENT_MUTEX_H_

#include "base/base_exports.h"

#include <pthread.h>

namespace base {

class ConditionVariable;

class BASE_EXPORT Mutex final {
 public:
  Mutex();

  // Once a mutex is created, it cannot be validly copied or moved to a new
  // location.
  Mutex(Mutex const&) = delete;
  Mutex& operator=(Mutex const&) = delete;

  // Acquire the ownership of this mutex.
  void Acquire();

  // Release the ownership of this mutex.
  void Release();

  // Try to acquire the ownership of this mutex.
  // a false return value means failed.
  bool TryAcquire();

  // Not virtual as base::Mutex is final.
  ~Mutex();

 private:
  friend base::ConditionVariable;
  pthread_mutex_t mtx_;
};
}  // namespace base
#endif