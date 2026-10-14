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

#if defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
#define IS_LINUX 1
constexpr bool const kIsLinux = 1;
#else
#define IS_LINUX 0
constexpr bool const kIsLinux = 0;
#endif  // defined(__linux) || defined(__linux__) || defined(__gnu_linux__)

#if defined(__APPLE__) || defined(__MACH__)
#define IS_MACOS 1
constexpr bool const kIsMacOS = 1;
#else
#define IS_MACOS 0
constexpr bool const kIsMacOS = 0;
#endif  // defined(__APPLE__) || defined(__MACH__)

#if defined(_WIN32) && !defined(_WIN64)
#define IS_WIN32 1
constexpr bool const kIsWin32 = 1;
#else
#define IS_WIN32 0
constexpr bool const kIsWin32 = 0;
#endif  // defined(_WIN32) && !defined(_WIN64)

#if defined(_WIN64)
#define IS_WIN64 1
constexpr bool const kIsWin64 = 1;
#else
#define IS_WIN64 0
constexpr bool const kIsWin64 = 0;
#endif  // defined(_WIN64)

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