/**
 * @file worker_thread.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-02-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_CONCURRENT_WORKER_THREAD_H_
#define BASE_CONCURRENT_WORKER_THREAD_H_

#include "base/concurrent/platform_thread_linux.h"

namespace base {
class WorkerThread : public PlatformThread::Delegate {
 public:
  enum WokerThreadState {
    IDLE = 0,
    BUSY = 1,
    HALT = 2,
  };
  WorkerThread() noexcept = default;

  void ThreadMain() override;

  bool IsStop() override;
};
}  // namespace base
#endif