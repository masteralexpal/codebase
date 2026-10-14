/**
 * @file thread.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-07-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/concurrent/thread.h"

#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <utility>

namespace base {

#if IS_LINUX && not defined(SYS_gettid)
#error "SYS_gettid unavailable on this system"
#endif
#define gettid() ((pid_t)syscall(SYS_gettid))

#if IS_LINUX
constexpr PlatformThreadHandle::KernelSpaceHandle const
    kInvalidKernelSpaceHandle = 0;
constexpr PlatformThreadHandle::UserSpaceHandle const kInvalidUserSpaceHandle =
    0LU;
#elif IS_MACOS
constexpr PlatformThreadHandle::KernelSpaceHandle const
    kInvalidKernelSpaceHandle = 0LU;
constexpr PlatformThreadHandle::UserSpaceHandle const kInvalidUserSpaceHandle =
    0LU;
#endif

// static
bool CurrentThread::IsMainThread() noexcept {
#if IS_LINUX
  return getpid() == gettid();
#elif IS_MACOS
  return pthread_main_np();
#endif
}

Thread::Thread(std::string const& name)
    : platform_thread_handle_(),
      platform_process_handle_(getpid()),
      state_{kCreated},
      joined_(false),
      name_(name) {
  platform_thread_handle_.kernel_space_handle = kInvalidKernelSpaceHandle;
  platform_thread_handle_.user_space_handle = kInvalidUserSpaceHandle;
}

Thread::Thread(Thread&& other) noexcept
    : platform_thread_handle_(std::move(other.platform_thread_handle_)),
      platform_process_handle_(std::move(other.platform_process_handle_)),
      state_(std::move(other.state_)),
      joined_(std::move(other.joined_)),
      name_(std::move(other.name_)) {}

Thread& Thread::operator=(Thread&& other) noexcept {
  platform_thread_handle_ = std::move(other.platform_thread_handle_);
  platform_process_handle_ = std::move(other.platform_process_handle_);
  state_ = std::move(other.state_);
  joined_ = std::move(other.joined_);
  name_ = std::move(other.name_);
  return *this;
}

Thread::~Thread() {
#if IS_LINUX || IS_MACOS
  if (!joined_ && state_ == kStarted) {
    pthread_join(platform_thread_handle_.user_space_handle, nullptr);
  }
#endif
}

void Thread::Join() noexcept {
  if (!joined_ && kStarted) {
    pthread_join(platform_thread_handle_.user_space_handle, nullptr);
    joined_ = true;
  }
}

std::string Thread::GetName() const noexcept {
  return name_;
}

Thread::State Thread::GetSate() const noexcept {
  return state_;
}

// private
// static
void* Thread::StartInternal(void* args) {
  auto* parameters = static_cast<StartRoutineParameters*>(args);
#if IS_LINUX
  parameters->thread->platform_thread_handle_.kernel_space_handle = gettid();
  pthread_setname_np(
      parameters->thread->platform_thread_handle_.user_space_handle,
      parameters->thread->name_.c_str());
#elif IS_MACOS
  pthread_threadid_np(
      parameters->thread->platform_thread_handle_.user_space_handle,
      parameters->thread->platform_thread_handle_.kernel_space_handle);
  pthread_setname_np(parameters->thread->name_.c_str());
#endif

  if (parameters && parameters->start_routine) {
    parameters->start_routine->operator()();
  }

  if (parameters->thread) {
    parameters->thread->SwitchState(kFinished);
  }

  delete parameters->start_routine;
  delete parameters;
  return nullptr;
}

// private
void Thread::SwitchState(State next_state) {
  CheckState(state_, next_state);
  state_ = next_state;
}

void Thread::CheckState(State currnet_state, State next_state) {
  switch (currnet_state) {
    case kCreated: {
      if (next_state != kStarted) {
        // TODO, Using stack strace.
        std::abort();
      }
      break;
    }
    case kStarted: {
      if (next_state == kCreated) {
        std::abort();
      }
      break;
    }
    case kSleeping: {
      if (next_state == kCreated) {
        std::abort();
      }
      break;
    }
    case kFinished: {
      std::abort();
      break;
    }
    default: {
      std::abort();
      break;
    }
  }
}

}  // namespace base