/**
 * @file spin_lock.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/concurrent/spin_lock.h"

#include <atomic>

namespace base {

// The C++20 default constructor of std::atomic_flag initializes to the
// clear state; do not use is_exclusive_(false) (P0883 ctor), which MSVC
// STL does not implement.
SpinLock::SpinLock() noexcept {}

void SpinLock::Acquire() noexcept {
  while (is_exclusive_.test_and_set(std::memory_order_acquire)) {
    // Spinning.
  }
}

void SpinLock::Release() noexcept {
  is_exclusive_.clear(std::memory_order_release);
}

}  // namespace base
