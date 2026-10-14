/**
 * @file mutex.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-07-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/concurrent/mutex.h"

#include <pthread.h>

namespace base {
Mutex::Mutex() : mtx_() {
  // Using default mutex attribute.
  pthread_mutex_init(&mtx_, nullptr);
};

void Mutex::Acquire() {
  pthread_mutex_lock(&mtx_);
}

void Mutex::Release() {
  pthread_mutex_unlock(&mtx_);
}

bool Mutex::TryAcquire() {
  return pthread_mutex_trylock(&mtx_) == 0;
}

Mutex::~Mutex() {
  pthread_mutex_destroy(&mtx_);
}
}  // namespace base