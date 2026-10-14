/**
 * @file monitor.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/concurrent/monitor.h"

namespace base {
Monitor::Monitor() {
  // default attr.
  pthread_mutex_init(&mtx_, nullptr);
  pthread_cond_init(&cv_, nullptr);
}

// TODO: Wrong impl.
void Monitor::Wait() {
  pthread_mutex_lock(&mtx_);
  pthread_cond_wait(&cv_, &mtx_);
}

void Monitor::NotifyAll() {
  pthread_cond_broadcast(&cv_);
}

void Monitor::NotifyOne() {
  pthread_cond_signal(&cv_);
}

Monitor::~Monitor() {
  pthread_cond_destroy(&cv_);
  pthread_mutex_destroy(&mtx_);
}
}  // namespace base