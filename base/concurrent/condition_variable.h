#ifndef BASE_CONCURRENT_CONDITION_VARIABLE_H_
#define BASE_CONCURRENT_CONDITION_VARIABLE_H_

#include "base/base_exports.h"
#include "base/concurrent/mutex.h"

#include <pthread.h>

namespace base {
class BASE_EXPORT ConditionVariable final {
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