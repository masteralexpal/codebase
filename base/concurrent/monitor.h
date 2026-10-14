/**
 * @file monitor.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONCURRENT_MONITOR_H_
#define BASE_CONCURRENT_MONITOR_H_

#include "base/base_exports.h"

#include <pthread.h>

namespace base {
// A monitor consists of a mutex (lock) object and condition variables.
// https://en.wikipedia.org/wiki/Monitor_(synchronization)
class BASE_EXPORT Monitor final {
 public:
  Monitor();

  Monitor(Monitor const&) = delete;
  Monitor& operator=(Monitor const&) = delete;

  void Wait();

  void NotifyAll();

  void NotifyOne();

  ~Monitor();

 private:
  pthread_mutex_t mtx_;
  pthread_cond_t cv_;
};
}  // namespace base
#endif