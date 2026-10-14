/**
 * @file spin_lock.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONCURRENT_SPIN_LOCK_H_
#define BASE_CONCURRENT_SPIN_LOCK_H_

#include <atomic>

#include "base/macro.h"

namespace base {
class SpinLock final {
 public:
  SpinLock() noexcept;

  DISABLE_COPY(SpinLock);

  DISABLE_MOVE(SpinLock);

  ~SpinLock() noexcept = default;

  void Acquire() noexcept;

  void Release() noexcept;

 private:
  std::atomic_flag is_exclusive_;
};
}  // namespace base
#endif
