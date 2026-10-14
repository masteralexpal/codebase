/**
 * @file restrict_mutex.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-07-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONCURRENT_RESTRICT_MUTEX_H_
#define BASE_CONCURRENT_RESTRICT_MUTEX_H_

#include "base/concurrent/mutex_guard.h"

#include <pthread.h>

#include "base/macro.h"

namespace base {
// Restrict mutex has private Acquire and Release.
// Must be used with base::MutextGuard<RestrictMutex>.
class API RestrictMutex final {
 public:
  RestrictMutex();

  RestrictMutex(RestrictMutex const&) = delete;
  RestrictMutex& operator=(RestrictMutex const&) = delete;

  ~RestrictMutex();

 private:
  friend class MutexGuard<RestrictMutex>;
  void Acquire();

  void Release();

  pthread_mutex_t pm_;
};
}  // namespace base
#endif