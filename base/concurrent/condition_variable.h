#ifndef BASE_CONCURRENT_CONDITION_VARIABLE_H_
#define BASE_CONCURRENT_CONDITION_VARIABLE_H_

#include "base/concurrent/mutex.h"
#include "base/macro.h"

#include <pthread.h>

namespace base {
class API ConditionVariable final {
 public:
  ConditionVariable();

  ConditionVariable(ConditionVariable const&) = delete;
  ConditionVariable& operator=(ConditionVariable const&) = delete;

  void Wait(base::Mutex& mutex);

  void Signal();

  void Broadcast();

  ~ConditionVariable();

 private:
  pthread_cond_t cv_;
};
}  // namespace base
#endif