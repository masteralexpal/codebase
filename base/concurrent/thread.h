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

#include <pthread.h>
#include <time.h>

#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <memory>
#include <semaphore>
#include <string>
#include <system_error>
#include <tuple>
#include <utility>

#include "base/base_exports.h"

namespace base {

/**
 * @brief pthread identity of a spawned thread: the user-space handle
 * (pthread_t, valid until join/detach) plus the kernel-space thread id
 * (as in gettid(2)).
 *
 */
struct PlatformThreadHandle {
  using KernelSpaceHandle = pid_t;
  using UserSpaceHandle = pthread_t;
  KernelSpaceHandle kernel_space_handle{};
  UserSpaceHandle user_space_handle{};
};

/**
 * @brief Functionalities that related to the current thread.
 *
 */
struct BASE_EXPORT CurrentThread {
  /**
   * @brief
   *
   * @return true
   * @return false
   */
  static bool IsMainThread() noexcept;

  /**
   * @brief The pthread_t of the calling thread.
   *
   */
  static PlatformThreadHandle::UserSpaceHandle GetHandle() noexcept;

  /**
   * @brief The kernel-space thread id (gettid(2)) of the calling thread.
   * Cached per thread: gettid() is a real syscall.
   *
   */
  static PlatformThreadHandle::KernelSpaceHandle GetKernelHandle() noexcept;

  template <typename Rep, typename Period>
  static void SleepFor(
      std::chrono::duration<Rep, Period> const& sleep_duration) {
    if (sleep_duration <= sleep_duration.zero()) {
      return;
    }
    auto seconds =
        std::chrono::duration_cast<std::chrono::seconds>(sleep_duration);
    auto nano_seconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
        sleep_duration - seconds);
    timespec time_spec = {static_cast<std::time_t>(seconds.count()),
                          static_cast<long>(nano_seconds.count())};
    while (::nanosleep(&time_spec, &time_spec) == -1 && errno == EINTR) {
    }
  }

  template <typename Clock, typename Duration>
  static void SleepUntil(
      std::chrono::time_point<Clock, Duration> const& wakeup_time_point) {
    auto now = Clock::now();
    if (Clock::is_steady) {
      if (now < wakeup_time_point)
        SleepFor(wakeup_time_point - now);
      return;
    }
    // using while-loop to deal with the unsteady clock.
    while (now < wakeup_time_point) {
      SleepFor(wakeup_time_point - now);
      now = Clock::now();
    }
  }
};

/**
 * @brief A naive wrapper of platform thread.
 *
 * Ownership policy:
 *  - A successfully Start()ed Thread owns a joinable pthread. Exactly one of
 *    Join(), Detach() or the destructor reclaims it.
 *  - The destructor joins (unlike std::thread, which terminates): a
 *    finished-but-unjoined pthread still holds resources that only
 *    pthread_join() releases. Joining from the owned thread itself aborts
 *    (it would deadlock).
 *  - Join()/Detach() on a non-joinable thread (never started, already
 *    joined/detached, moved-from) aborts: that is a logic error, not a
 *    runtime condition.
 *
 * Threading notes:
 *  - The State lives in a control block shared with the child thread, so the
 *    child never dereferences `this` after the startup handshake. Moving (or
 *    destroying, after Detach()) a Thread while its child is still running
 *    is therefore safe.
 *  - Start() blocks until the child has published its kernel-space handle
 *    and its name, so both are valid as soon as Start() returns.
 *
 */
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
    int const result =
        pthread_create(&platform_thread_handle_.user_space_handle, nullptr,
                       &StartInternal, parameters);
    if (result != 0) {
      // Roll back to kCreated so Start() may be retried.
      state_->store(kCreated, std::memory_order_relaxed);
      delete parameters;
      throw std::system_error(result, std::generic_category(),
                              "pthread_create");
    }
    joinable_ = true;

    // Wait until the child has published its kernel-space handle and name.
    // StartInternal() must not touch `parameters` after releasing `started`.
    parameters->started.acquire();
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
  // between pthread_create() and the `started` handshake, when it is lent to
  // StartInternal().
  struct StartRoutineParameters {
    std::shared_ptr<std::atomic<State>> state;
    std::string name;
    StartRoutine start_routine;
    std::binary_semaphore started{0};
    PlatformThreadHandle::KernelSpaceHandle kernel_handle{};
  };

  static void* StartInternal(void* args);

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
