/**
 * @file platform_thread_linux.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_CONCURRENT_PLATFORM_THREAD_LINUX_H_
#define BASE_CONCURRENT_PLATFORM_THREAD_LINUX_H_

#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <string>

namespace base {

class PlatformThread {
 public:
  using Handle = uint64_t;
  using KernelHandle = int32_t;

  class Delegate {
   public:
    virtual void ThreadMain() = 0;
  };

  struct Attributes {
    // in bytes.
    size_t stack_size = 1024 * 1024 * 8;
  };

  static int Spawn(Delegate* delegate,
                   Handle* handle = nullptr,
                   Attributes* attributes = nullptr);

  static int Join(Handle handle);

  static int Detach(Handle handle);

  static int SetName(Handle handle, std::string const& name);

  static int GetName(Handle handle, std::string& name);

  class Current {
   public:
    static Handle GetHandle() noexcept;

    static KernelHandle GetKernelHandle() noexcept;

    static Delegate* GetDelegate() noexcept;

    static bool IsMainThread() noexcept;

    static int Yield() noexcept;

    static int SetName(std::string const& name);

    static int GetName(std::string& name);

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
};

}  // namespace base
#endif  // BASE_CONCURRENT_PLATFORM_THREAD_LINUX_H_