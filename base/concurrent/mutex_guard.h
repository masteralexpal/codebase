#ifndef BASE_CONCURRENT_MUTEX_GUARD_H_
#define BASE_CONCURRENT_MUTEX_GUARD_H_

#include "base/macro.h"

namespace base {
template <typename MutexType>
// No export macro: templates are instantiated in the consuming TU.
class MutexGuard final {
 public:
  explicit MutexGuard(MutexType& mutex) : mutex_(mutex) { mutex_.Acquire(); }

  DISABLE_COPY(MutexGuard);
  DISABLE_MOVE(MutexGuard);

  ~MutexGuard() { mutex_.Release(); }

 private:
  MutexType& mutex_;
};
}  // namespace base
#endif