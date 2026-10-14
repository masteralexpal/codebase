/**
 * @file thread_win.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief Win32 backend of base::Thread and base::CurrentThread (see
 * thread.cc for the platform-neutral logic).
 * @version 0.1
 * @date 2026-10-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "base/concurrent/thread.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <process.h>

#include <cerrno>
#include <cstddef>
#include <string>
#include <system_error>

namespace base {

namespace {

// Captured during dynamic initialization, which runs on the main thread
// (before main() for an executable). Caveat: if base is linked into a DLL,
// this captures the DLL-loading thread instead.
DWORD const g_main_thread_id = ::GetCurrentThreadId();

}  // namespace

// static
bool CurrentThread::IsMainThread() noexcept {
  return ::GetCurrentThreadId() == g_main_thread_id;
}

// static
PlatformThreadHandle::KernelSpaceHandle CurrentThread::GetKernelHandle()
    noexcept {
  // Unlike gettid(2), GetCurrentThreadId() is a plain TEB read, so there is
  // nothing worth caching.
  return ::GetCurrentThreadId();
}

// static
void CurrentThread::SetName(std::string const& name) noexcept {
  // SetThreadDescription (Windows 10 version 1607+) takes UTF-16.
  int const length = ::MultiByteToWideChar(CP_UTF8, 0, name.data(),
                                           static_cast<int>(name.size()),
                                           nullptr, 0);
  if (length <= 0) {
    return;
  }
  std::wstring wide_name(static_cast<std::size_t>(length), L'\0');
  ::MultiByteToWideChar(CP_UTF8, 0, name.data(),
                        static_cast<int>(name.size()), wide_name.data(),
                        length);
  // Best effort: a naming failure must not kill the child thread.
  static_cast<void>(
      ::SetThreadDescription(::GetCurrentThread(), wide_name.c_str()));
}

// static
unsigned int __stdcall Thread::StartInternalAdapter(void* args) {
  StartInternal(args);
  return 0;
}

// static
void Thread::Spawn(StartRoutineParameters* parameters) {
  // _beginthreadex, NOT CreateThread: a thread started with CreateThread
  // that touches the CRT leaks its per-thread CRT state (errno, strtok
  // context, ...) on exit. _beginthreadex allocates and releases it.
  unsigned int thread_id = 0;
  uintptr_t const raw_handle =
      _beginthreadex(nullptr, 0, &StartInternalAdapter, parameters, 0,
                     &thread_id);
  if (raw_handle == 0) {
    // _beginthreadex reports failures through errno.
    throw std::system_error(errno, std::generic_category(),
                            "_beginthreadex");
  }
  parameters->user_handle =
      reinterpret_cast<PlatformThreadHandle::UserSpaceHandle>(raw_handle);
}

// static
void Thread::PlatformJoin(PlatformThreadHandle::UserSpaceHandle handle)
    noexcept {
  // Joining consumes the handle: wait, then close.
  static_cast<void>(::WaitForSingleObject(handle, INFINITE));
  ::CloseHandle(handle);
}

// static
void Thread::PlatformDetach(PlatformThreadHandle::UserSpaceHandle handle)
    noexcept {
  // A Win32 thread detaches by simply closing the handle: the kernel object
  // stays alive on the thread's own reference until it exits.
  ::CloseHandle(handle);
}

// static
bool Thread::PlatformIsSelf(PlatformThreadHandle::UserSpaceHandle handle)
    noexcept {
  // HANDLE values themselves are not comparable (one thread may own several
  // different handles); compare thread IDs instead.
  return ::GetThreadId(handle) == ::GetCurrentThreadId();
}

}  // namespace base
