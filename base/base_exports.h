/**
 * @file base_exports.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-07-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_BASE_EXPORTS_H_
#define BASE_BASE_EXPORTS_H_

// Symbol visibility for the base library.
//
// Windows (cl / clang-cl): symbols are hidden by default and must be exported
// explicitly with __declspec(dllexport) when building the DLL, and imported
// with __declspec(dllimport) when consuming it. BASE_IMPLEMENTATION is defined
// by the build system (base/CMakeLists.txt) only while compiling base itself.
//
// ELF platforms (GCC / Clang): symbols are visible by default; BASE_EXPORT
// marks them explicitly so the library also works with -fvisibility=hidden.
//
// BASE_STATIC_DEFINE provides an escape hatch for linking base statically on
// Windows, where dllimport would be wrong.
#if defined(BASE_STATIC_DEFINE)
#define BASE_EXPORT
#elif defined(_WIN32)
#if defined(BASE_IMPLEMENTATION)
#define BASE_EXPORT __declspec(dllexport)
#else
#define BASE_EXPORT __declspec(dllimport)
#endif  // defined(BASE_IMPLEMENTATION)
#else
#define BASE_EXPORT __attribute__((visibility("default")))
#endif

#endif  // BASE_BASE_EXPORTS_H_