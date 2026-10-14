/**
 * @file platform.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_PLATFORM_PLATFORM_H_
#define BASE_PLATFORM_PLATFORM_H_

#include "base/build_config.h"

#if IS_LINUX || IS_MACOS
#include <sys/types.h>
using PlatformProcessHandle = pid_t;
using PlatformThreadHandle = pid_t;
#elif IS_WIN32 || IS_WIN64
#error "Windows is not supported yet."
#endif  // IS_LINUX || IS_MACOS

namespace base {
PlatformProcessHandle GetPlatformProcessHandle() noexcept;

PlatformThreadHandle GetPlatformThreadHandle() noexcept;

}  // namespace base

#endif  // BASE_PLATFORM_PLATFORM_H_