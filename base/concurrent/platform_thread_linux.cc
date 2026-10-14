/**
 * @file platform_thread_linux.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "base/concurrent/platform_thread_linux.h"

#include <pthread.h>
#include <sched.h>
#include <syscall.h>
#include <unistd.h>

#include <cerrno>

#include "base/log.h"

namespace base {

#if not defined(SYS_gettid)
#error "SYS_gettid unavailable on this system"
#endif
#define gettid() ((pid_t)syscall(SYS_gettid))

thread_local PlatformThread::Delegate* tls_delegate = nullptr;

static void* StartRoutine(void* data) {
  if (!data)
    return nullptr;

  auto* delegate = static_cast<PlatformThread::Delegate*>(data);

  tls_delegate = delegate;

  delegate->ThreadMain();

  return nullptr;
}

int PlatformThread::Spawn(Delegate* delegate,
                          Handle* handle,
                          Attributes* attributes) {
  if (!delegate || !handle) {
    return EINVAL;
  }

  if (!attributes) {
    static Attributes default_attributes;
    attributes = &default_attributes;
  }

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, attributes->stack_size);

  int rv = pthread_create(handle, &attr, StartRoutine, delegate);

  pthread_attr_destroy(&attr);

  switch (rv) {
    case EAGAIN:
      return rv;
    case EINVAL:
      return rv;
    case EPERM:
      return rv;
    default:
      return 0;
  }
}

int PlatformThread::Join(Handle handle) {
  return pthread_join(handle, nullptr);
}

int PlatformThread::Detach(Handle handle) {
  return pthread_detach(handle);
}

int PlatformThread::SetName(Handle handle, std::string const& name) {
  return pthread_setname_np(handle, name.c_str());
}

int PlatformThread::GetName(Handle handle, std::string& name) {
  static char buff[1024];
  int rv = pthread_getname_np(handle, buff, sizeof(buff));
  name = std::string{buff};
  return rv;
}

PlatformThread::Handle PlatformThread::Current::GetHandle() noexcept {
  thread_local PlatformThread::Handle thread_handle = pthread_self();
  return thread_handle;
}

PlatformThread::KernelHandle
PlatformThread::Current::GetKernelHandle() noexcept {
  thread_local PlatformThread::KernelHandle kernel_handle = gettid();
  return kernel_handle;
}

PlatformThread::Delegate* PlatformThread::Current::GetDelegate() noexcept {
  return tls_delegate;
}

bool PlatformThread::Current::IsMainThread() noexcept {
  thread_local bool is_main_thread = (GetKernelHandle() == getpid());
  return is_main_thread;
}

int PlatformThread::Current::Yield() noexcept {
  return sched_yield();
}

int PlatformThread::Current::SetName(std::string const& name) {
  return PlatformThread::SetName(Current::GetHandle(), name.c_str());
}

int PlatformThread::Current::GetName(std::string& name) {
  return PlatformThread::GetName(Current::GetHandle(), name);
}

}  // namespace base