/**
 * @file platform.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "base/platform/platform.h"

#include <sys/syscall.h>
#include <unistd.h>

namespace base {
PlatformProcessHandle GetPlatformProcessHandle() noexcept {
#if IS_LINUX || IS_MACOS
  PlatformProcessHandle handle = getpid();
  return handle;
#endif
}

PlatformThreadHandle GetPlatformThreadHandle() noexcept {
#if IS_LINUX
#ifndef SYS_gettid
#error "SYS_gettid unavailable on this platform"
#endif
  thread_local PlatformThreadHandle handle = syscall(SYS_gettid);
  return handle;
#endif
}

}  // namespace base
