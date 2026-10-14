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

#include <chrono>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include "base/macro.h"

namespace base {

/**
 * @brief Functionalities that related to the current thread.
 *
 */
struct API CurrentThread {
  /**
   * @brief
   *
   * @return true
   * @return false
   */
  static bool IsMainThread() noexcept;

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
 * @brief
 *
 */
struct PlatformThreadHandle {
#if IS_LINUX
  using KernelSpaceHandle = pid_t;
  using UserSpaceHandle = pthread_t;
#elif IS_MACOS
  using KernelSpaceHandle = uint64_t;
  using UserSpaceHandle = pthread_t;
#endif
  KernelSpaceHandle kernel_space_handle{};
  UserSpaceHandle user_space_handle{};
};

struct PlatformProcessHandle {
#if IS_LINUX || IS_MACOS
  using ProcessHandle = pid_t;
  inline PlatformProcessHandle() = default;

  inline explicit PlatformProcessHandle(
      ProcessHandle const& process_handle) noexcept
      : process_handle(process_handle) {}
  ProcessHandle process_handle{};
#endif
};

/**
 * @brief A naive wrapper of platform thread.
 *
 */
class API Thread {
 public:
  // using PlatformThread = pthread_t;

  enum State {
    // Create but not started yet.
    kCreated,
    // After Thread::Start() called.
    kStarted,
    // Block
    // kBlocked,
    // Sleeping.
    kSleeping,
    // StartRoutine execution finished.
    kFinished,
  };

  explicit Thread(std::string const& name = "Anonymous");

  // Disable copy.
  DISABLE_COPY(Thread);
  // Support move.
  Thread(Thread&&) noexcept;
  Thread& operator=(Thread&&) noexcept;

  virtual ~Thread();

  template <typename Callable, typename... Args>
  void Start(Callable&& callable, Args&&... args) {
    SwitchState(kStarted);

    StartRoutineParameters* parameters = new StartRoutineParameters;

    parameters->start_routine = new StartRoutine(
        // explicitly copy callable and args...
        [callable_(std::forward<Callable>(callable)),
         args_(std::make_tuple(std::forward<Args>(
             args)...))]() mutable -> void { std::apply(callable_, args_); });

    parameters->thread = this;

    pthread_create(&platform_thread_handle_.user_space_handle, nullptr,
                   &StartInternal, parameters);
  }

  void Join() noexcept;

  void Detach() noexcept;

  std::string GetName() const noexcept;

  State GetSate() const noexcept;

 private:
  // Test helper function.
  friend class ThreadTestHelper;

  using StartRoutine = std::function<void()>;

  struct StartRoutineParameters {
    Thread* thread;
    StartRoutine* start_routine;
  };

  static void* StartInternal(void* args);
  /**
   * @brief Switch thread state.
   *
   * @param next_state - the next state of base::Thread.
   */
  void SwitchState(State next_state);

  /**
   * @brief check if the state transition is ok.
   *
   * @param currnet_state
   * @param next_state
   */
  void CheckState(State currnet_state, State next_state);

  PlatformThreadHandle platform_thread_handle_;
  PlatformProcessHandle platform_process_handle_;

  // State
  State state_;
  // joined
  bool joined_;
  // Unmutable.
  std::string name_;
};

}  // namespace base
#endif