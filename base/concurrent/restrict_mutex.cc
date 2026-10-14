/**
 * @file restrict_mutex.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-07-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/concurrent/restrict_mutex.h"

namespace base {
RestrictMutex::RestrictMutex() : pm_() {
  pthread_mutex_init(&pm_, nullptr);
}

void RestrictMutex::Acquire() {
  pthread_mutex_lock(&pm_);
}

void RestrictMutex::Release() {
  pthread_mutex_unlock(&pm_);
}

RestrictMutex::~RestrictMutex() {
  pthread_mutex_destroy(&pm_);
}
}  // namespace base