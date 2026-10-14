/**
 * @file macro.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_MACRO_H_
#define BASE_MACRO_H_

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
constexpr bool const kIsWin64 = 1;
#endif  // defined(_WIN64)

// Symbols.
#if IS_LINUX
#define API __attribute__((visibility("default")))
#endif

#endif  // BASE_MACRO_H_