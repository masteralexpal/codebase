/**
 * @file thread_local.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <gtest/gtest.h>

#include <pthread.h>
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <cstdio>
#include <thread>
#include <tuple>
#include <vector>

inline pid_t GetThreadID() {
  std::puts("GetThreadID created!");
  return syscall(SYS_gettid);
}

class PlatformThreadHandle {
 public:
  // inline PlatformThreadHandle() noexcept {
  //   std::puts("PlatformThreadHandle created!");
  //   id_ = syscall(SYS_gettid);
  // }

  inline PlatformThreadHandle(pid_t thread_id) : id_(thread_id) {
    std::puts("PlatformThreadHandle created!");
  }

  inline ~PlatformThreadHandle() noexcept {
    std::puts("PlatformThreadHandle destroyed!");
  }

  inline pid_t Get() const noexcept { return id_; }

 private:
  pid_t id_{};
};

void threda_func() {
  for (int i = 0; i < 100; ++i) {
    thread_local PlatformThreadHandle handle = GetThreadID();
    std::ignore = handle.Get();
  }
}

TEST(a, b) {
  std::puts("thread created!");
  std::vector<std::thread> threads;
  for (int i = 0; i < 4; ++i) {
    threads.emplace_back(threda_func);
  }

  for (auto&& thread : threads) {
    thread.join();
  }

  std::puts("thread destroyed!");
}