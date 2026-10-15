/**
 * @file build_config.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief Operating system detection macros for #if dispatch.
 * @version 0.1
 * @date 2024-01-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_BUILD_CONFIG_H_
#define BASE_BUILD_CONFIG_H_

// OS detection. These are macros (not constexpr booleans) because their only
// legitimate use is preprocessor conditional compilation.
#if defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
#define CODEBASE_IS_LINUX 1
#else
#define CODEBASE_IS_LINUX 0
#endif  // defined(__linux) || defined(__linux__) || defined(__gnu_linux__)

#if defined(__APPLE__) || defined(__MACH__)
#define CODEBASE_IS_MACOS 1
#else
#define CODEBASE_IS_MACOS 0
#endif  // defined(__APPLE__) || defined(__MACH__)

// Windows, any bitness. _WIN32 is defined on both 32- and 64-bit Windows
// (the "Win32" API name covers both); use CODEBASE_IS_WINDOWS for "are we on
// Windows" and CODEBASE_IS_WIN32/CODEBASE_IS_WIN64 below only when the bitness matters.
#if defined(_WIN32)
#define CODEBASE_IS_WINDOWS 1
#else
#define CODEBASE_IS_WINDOWS 0
#endif  // defined(_WIN32)

#if defined(_WIN32) && !defined(_WIN64)
#define CODEBASE_IS_WIN32 1
#else
#define CODEBASE_IS_WIN32 0
#endif  // defined(_WIN32) && !defined(_WIN64)

#if defined(_WIN64)
#define CODEBASE_IS_WIN64 1
#else
#define CODEBASE_IS_WIN64 0
#endif  // defined(_WIN64)

#endif  // BASE_BUILD_CONFIG_H_
