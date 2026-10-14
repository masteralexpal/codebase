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
#include <unistd.h>

#include <atomic>
#include <cstdlib>
#include <exception>
#include <memory>
#include <string>
#include <utility>

namespace base {

namespace {

constexpr PlatformThreadHandle::KernelSpaceHandle const
    kInvalidKernelSpaceHandle = 0;
constexpr PlatformThreadHandle::UserSpaceHandle const kInvalidUserSpaceHandle =
    0LU;

// pthread_setname_np(3) limits thread names to 16 bytes including the
// terminating NUL.
constexpr std::size_t const kMaxThreadNameLength = 15;

void SetCurrentThreadName(std::string const& name) {
  std::string const truncated = name.substr(0, kMaxThreadNameLength);
  // Best effort: a naming failure must not kill the child thread.
  static_cast<void>(pthread_setname_np(pthread_self(), truncated.c_str()));
}

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

Thread::Thread(std::string const& name)
    : platform_thread_handle_{},
      state_(std::make_shared<std::atomic<State>>(kCreated)),
      joinable_(false),
      name_(name) {
  platform_thread_handle_.kernel_space_handle = kInvalidKernelSpaceHandle;
  platform_thread_handle_.user_space_handle = kInvalidUserSpaceHandle;
}

Thread::Thread(Thread&& other) noexcept
    : platform_thread_handle_(other.platform_thread_handle_),
      state_(std::move(other.state_)),
      joinable_(other.joinable_),
      name_(std::move(other.name_)) {
  other.platform_thread_handle_.kernel_space_handle = kInvalidKernelSpaceHandle;
  other.platform_thread_handle_.user_space_handle = kInvalidUserSpaceHandle;
  other.joinable_ = false;
}

Thread& Thread::operator=(Thread&& other) noexcept {
  if (this != &other) {
    // Give up the currently owned thread first, with the same policy as the
    // destructor.
    if (joinable_) {
      if (pthread_equal(pthread_self(),
                        platform_thread_handle_.user_space_handle)) {
        std::abort();
      }
      pthread_join(platform_thread_handle_.user_space_handle, nullptr);
    }
    platform_thread_handle_ = other.platform_thread_handle_;
    state_ = std::move(other.state_);
    joinable_ = other.joinable_;
    name_ = std::move(other.name_);
    other.platform_thread_handle_.kernel_space_handle =
        kInvalidKernelSpaceHandle;
    other.platform_thread_handle_.user_space_handle = kInvalidUserSpaceHandle;
    other.joinable_ = false;
  }
  return *this;
}

Thread::~Thread() {
  if (!joinable_) {
    return;
  }
  if (pthread_equal(pthread_self(),
                    platform_thread_handle_.user_space_handle)) {
    // Joining ourselves would deadlock.
    std::abort();
  }
  // Also reclaims a kFinished thread: its resources are held until
  // pthread_join().
  pthread_join(platform_thread_handle_.user_space_handle, nullptr);
}

void Thread::Join() noexcept {
  if (!joinable_) {
    // Never started, already joined/detached, or moved-from.
    std::abort();
  }
  if (pthread_equal(pthread_self(),
                    platform_thread_handle_.user_space_handle)) {
    // Self-join deadlocks.
    std::abort();
  }
  pthread_join(platform_thread_handle_.user_space_handle, nullptr);
  joinable_ = false;
  platform_thread_handle_.user_space_handle = kInvalidUserSpaceHandle;
}

void Thread::Detach() noexcept {
  if (!joinable_) {
    // Never started, already joined/detached, or moved-from.
    std::abort();
  }
  // Detaching ourselves (pthread_detach(pthread_self())) is fine.
  pthread_detach(platform_thread_handle_.user_space_handle);
  joinable_ = false;
  platform_thread_handle_.user_space_handle = kInvalidUserSpaceHandle;
}

std::string Thread::GetName() const noexcept {
  return name_;
}

Thread::State Thread::GetState() const noexcept {
  if (!state_) {
    // Moved-from.
    return kCreated;
  }
  return state_->load(std::memory_order_acquire);
}

// static
void* Thread::StartInternal(void* args) {
  auto* parameters = static_cast<StartRoutineParameters*>(args);

  // Move out everything the routine needs up front: after started.release(),
  // `parameters` belongs to Start() again and must not be touched here.
  std::shared_ptr<std::atomic<State>> const state =
      std::move(parameters->state);
  StartRoutine const start_routine = std::move(parameters->start_routine);

  parameters->kernel_handle = ::gettid();
  SetCurrentThreadName(parameters->name);
  parameters->started.release();

  try {
    start_routine();
  } catch (...) {
    // A start routine must not let an exception escape its thread
    // (same contract as std::thread).
    std::terminate();
  }
  state->store(kFinished, std::memory_order_release);
  return nullptr;
}

}  // namespace base
