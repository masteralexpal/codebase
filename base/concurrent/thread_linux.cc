/**
 * @file thread_linux.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief POSIX/pthread backend of base::Thread and base::CurrentThread
 * (see thread.cc for the platform-neutral logic).
 * @version 0.1
 * @date 2023-07-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/concurrent/thread.h"

#include <pthread.h>
#include <unistd.h>

#include <cstddef>
#include <string>
#include <system_error>

namespace base {

namespace {

// pthread_setname_np(3) limits thread names to 16 bytes including the
// terminating NUL.
constexpr std::size_t const kMaxThreadNameLength = 15;

}  // namespace

// static
bool CurrentThread::IsMainThread() noexcept {
  return ::getpid() == ::gettid();
}

// static
PlatformThreadHandle::UserSpaceHandle CurrentThread::GetHandle() noexcept {
  return pthread_self();
}

// static
PlatformThreadHandle::KernelSpaceHandle CurrentThread::GetKernelHandle()
    noexcept {
  // gettid(2) is a real syscall (no vDSO); cache it per thread.
  thread_local PlatformThreadHandle::KernelSpaceHandle const kernel_handle =
      ::gettid();
  return kernel_handle;
}

// static
void CurrentThread::SetName(std::string const& name) noexcept {
  std::string const truncated = name.substr(0, kMaxThreadNameLength);
  // Best effort: a naming failure must not kill the child thread.
  static_cast<void>(pthread_setname_np(pthread_self(), truncated.c_str()));
}

// static
void Thread::Spawn(StartRoutineParameters* parameters) {
  int const result = pthread_create(&parameters->user_handle, nullptr,
                                    &StartInternal, parameters);
  if (result != 0) {
    throw std::system_error(result, std::generic_category(),
                            "pthread_create");
  }
}

// static
void Thread::PlatformJoin(PlatformThreadHandle::UserSpaceHandle handle)
    noexcept {
  pthread_join(handle, nullptr);
}

// static
void Thread::PlatformDetach(PlatformThreadHandle::UserSpaceHandle handle)
    noexcept {
  pthread_detach(handle);
}

// static
bool Thread::PlatformIsSelf(PlatformThreadHandle::UserSpaceHandle handle)
    noexcept {
  return pthread_equal(pthread_self(), handle);
}

}  // namespace base
