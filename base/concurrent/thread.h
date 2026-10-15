/**
 * @file thread.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-07-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONCURRENT_THREAD_H_
#define BASE_CONCURRENT_THREAD_H_

#include "base/build_config.h"

#if IS_LINUX
#include <pthread.h>
#endif

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <memory>
#include <semaphore>
#include <string>
#include <thread>
#include <tuple>
#include <utility>

#include "base/base_exports.h"

namespace base {

// OS identity of a thread: the user-space handle (valid until join/detach)
// plus the kernel-space thread id.
struct PlatformThreadHandle {
#if IS_WINDOWS
  // HANDLE and DWORD, spelled out so that this header does not have to drag
  // in <windows.h>.
  using UserSpaceHandle = void*;
  using KernelSpaceHandle = unsigned long;
#elif IS_LINUX
  using UserSpaceHandle = pthread_t;
  using KernelSpaceHandle = pid_t;
#endif
  KernelSpaceHandle kernel_space_handle{};
  UserSpaceHandle user_space_handle{};
};

// Functionalities related to the calling thread.
struct BASE_EXPORT CurrentThread {
  static bool IsMainThread() noexcept;

#if IS_LINUX
  // The pthread_t of the calling thread.
  //
  // Deliberately Linux-only: the Win32 GetCurrentThread() returns a
  // *pseudo*-handle (a constant meaning "this thread") that can neither be
  // handed to another thread nor waited on. Exposing it as a real handle is
  // a classic Win32 bug; GetKernelHandle() is the portable identity.
  static PlatformThreadHandle::UserSpaceHandle GetHandle() noexcept;
#endif

  // The kernel-space thread id of the calling thread (gettid(2) /
  // GetCurrentThreadId()).
  static PlatformThreadHandle::KernelSpaceHandle GetKernelHandle() noexcept;

  // Names the calling thread (best effort). Linux truncates to 15 bytes
  // (pthread_setname_np's 16-byte limit); Windows uses
  // SetThreadDescription (Windows 10 version 1607+).
  static void SetName(std::string const& name) noexcept;

  template <typename Rep, typename Period>
  static void SleepFor(
      std::chrono::duration<Rep, Period> const& sleep_duration) {
    std::this_thread::sleep_for(sleep_duration);
  }

  template <typename Clock, typename Duration>
  static void SleepUntil(
      std::chrono::time_point<Clock, Duration> const& wakeup_time_point) {
    std::this_thread::sleep_until(wakeup_time_point);
  }
};

// A naive wrapper of platform thread.
//
// Ownership policy:
//  - A successfully Start()ed Thread owns a joinable OS thread. Exactly one
//    of Join(), Detach() or the destructor reclaims it.
//  - The destructor joins (unlike std::thread, which terminates): a
//    finished-but-unjoined thread still holds resources that only the join
//    releases. Joining from the owned thread itself aborts (it would
//    deadlock).
//  - Join()/Detach() on a non-joinable thread (never started, already
//    joined/detached, moved-from) aborts: that is a logic error, not a
//    runtime condition.
//
// Threading notes:
//  - The State lives in a control block shared with the child thread, so the
//    child never dereferences `this` after the startup handshake. Moving (or
//    destroying, after Detach()) a Thread while its child is still running
//    is therefore safe.
//  - Start() blocks until the child has published its kernel-space handle
//    and its name, so both are valid as soon as Start() returns.
//
// Portability:
//  - The class logic is platform-neutral; only four small hooks (Spawn,
//    PlatformJoin, PlatformDetach, PlatformIsSelf) plus CurrentThread are
//    implemented per platform in thread_linux.cc and thread_win.cc.
class BASE_EXPORT Thread final {
 public:
  enum State {
    // Created but not started yet.
    kCreated,
    // After Thread::Start() called.
    kStarted,
    // StartRoutine execution finished.
    kFinished,
  };

  explicit Thread(std::string const& name = "Anonymous");

  // Disable copy.
  Thread(Thread const&) = delete;
  Thread& operator=(Thread const&) = delete;
  // Support move. A moved-from Thread is only good for destruction and
  // move-assignment (GetState() reports kCreated).
  Thread(Thread&&) noexcept;
  Thread& operator=(Thread&&) noexcept;

  ~Thread();

  template <typename Callable, typename... Args>
  void Start(Callable&& callable, Args&&... args) {
    if (!state_ || state_->load(std::memory_order_relaxed) != kCreated) {
      // Double Start(), or Start() on a moved-from Thread.
      std::abort();
    }

    auto* parameters = new StartRoutineParameters;
    parameters->state = state_;
    parameters->name = name_;
    parameters->start_routine = StartRoutine(
        // explicitly copy callable and args...
        [callable_(std::forward<Callable>(callable)),
         args_(std::make_tuple(std::forward<Args>(
             args)...))]() mutable -> void { std::apply(callable_, args_); });

    state_->store(kStarted, std::memory_order_relaxed);
    try {
      // Platform hook: spawns StartInternal(parameters) and publishes the
      // user-space handle into parameters->user_handle.
      Spawn(parameters);
    } catch (...) {
      // Roll back to kCreated so Start() may be retried.
      state_->store(kCreated, std::memory_order_relaxed);
      delete parameters;
      throw;
    }
    joinable_ = true;

    // Wait until the child has published its kernel-space handle and name.
    // StartInternal() must not touch `parameters` after releasing `started`.
    parameters->started.acquire();
    platform_thread_handle_.user_space_handle = parameters->user_handle;
    platform_thread_handle_.kernel_space_handle = parameters->kernel_handle;
    delete parameters;
  }

  void Join() noexcept;

  void Detach() noexcept;

  std::string GetName() const noexcept;

  State GetState() const noexcept;

 private:
  using StartRoutine = std::function<void()>;

  // Everything the child thread needs at startup. Owned by Start(), except
  // between Spawn() and the `started` handshake, when it is lent to
  // StartInternal().
  struct StartRoutineParameters {
    std::shared_ptr<std::atomic<State>> state;
    std::string name;
    StartRoutine start_routine;
    std::binary_semaphore started{0};
    PlatformThreadHandle::UserSpaceHandle user_handle{};
    PlatformThreadHandle::KernelSpaceHandle kernel_handle{};
  };

  static void* StartInternal(void* args);

  // Platform hooks, defined in thread_linux.cc / thread_win.cc.
  // Spawn() throws std::system_error on failure.
  static void Spawn(StartRoutineParameters* parameters);
  // PlatformJoin() blocks until the thread exits and releases the handle.
  static void PlatformJoin(
      PlatformThreadHandle::UserSpaceHandle handle) noexcept;
  static void PlatformDetach(
      PlatformThreadHandle::UserSpaceHandle handle) noexcept;
  static bool PlatformIsSelf(
      PlatformThreadHandle::UserSpaceHandle handle) noexcept;

#if IS_WINDOWS
  // _beginthreadex entry bridge: it requires an unsigned __stdcall(void*)
  // routine, StartInternal() uses the pthread signature.
  static unsigned int __stdcall StartInternalAdapter(void* args);
#endif

  PlatformThreadHandle platform_thread_handle_{};
  // Shared with the child thread so it can report kFinished even after this
  // object was moved or destroyed.
  std::shared_ptr<std::atomic<State>> state_;
  // Written only by the owner thread (Start/Join/Detach/destructor), so a
  // plain bool suffices.
  bool joinable_;
  // Unmutable.
  std::string name_;
};

}  // namespace base
#endif
