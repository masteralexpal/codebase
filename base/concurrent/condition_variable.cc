#include "base/concurrent/condition_variable.h"
#include <pthread.h>

namespace base {
ConditionVariable::ConditionVariable() {
  pthread_cond_init(&cv_, nullptr);
}

void ConditionVariable::Wait(base::Mutex& mutex) {
  // The pthread_cond_wait() routine always returns with the mutex locked and
  // owned by the calling thread, even when returning an error.
  pthread_cond_wait(&cv_, &mutex.mtx_);
}

void ConditionVariable::Signal() {
  pthread_cond_signal(&cv_);
}

void ConditionVariable::Broadcast() {
  pthread_cond_broadcast(&cv_);
}

ConditionVariable::~ConditionVariable() {
  pthread_cond_destroy(&cv_);
}
}  // namespace base