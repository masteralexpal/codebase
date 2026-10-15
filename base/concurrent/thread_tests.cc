/**
 * @file thread_tests.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-07-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/concurrent/thread.h"

#include <gtest/gtest.h>

#include "base/build_config.h"

#if CODEBASE_IS_LINUX
#include <unistd.h>
#endif

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <utility>

namespace base {
TEST(Thread, Creation) {
  Thread thread;
  ASSERT_STREQ(thread.GetName().c_str(), "Anonymous");
  ASSERT_EQ(thread.GetState(), Thread::kCreated);
}
void PlainFunction0() {
  std::cout << __func__ << "() called!";
}

TEST(Thread, StartWithPlainFunction0) {
  Thread thread("Main");
  ASSERT_EQ(thread.GetState(), Thread::kCreated);
  ASSERT_STREQ(thread.GetName().c_str(), "Main");
  thread.Start(PlainFunction0);
  // By the time Start() returns, the child may already have run to
  // completion: kStarted and kFinished are both legal here.
  ASSERT_NE(thread.GetState(), Thread::kCreated);
  thread.Join();
  // Join() returns after the start routine, hence after the kFinished store.
  ASSERT_EQ(thread.GetState(), Thread::kFinished);
}

void PlainFunction1(int a, int b, int* c) {
  *c = a + b;
}

TEST(Thread, StartWithPlainFunction1) {
  int a = 1;
  int b = 10;
  int result = 0;
  Thread thread;
  thread.Start(PlainFunction1, a, b, &result);
  thread.Join();
  ASSERT_EQ(result, a + b);
}

class Functor {
 public:
  void operator()() {}
};

TEST(Thread, StartWithFunctor) {
  Thread thread;
  thread.Start(Functor{});
  thread.Join();
}

TEST(Thread, Detach) {
  Thread thread;
  thread.Start([]() -> void {});
  thread.Detach();
  // The destructor must not join a detached thread.
}

TEST(Thread, DestructorJoins) {
  bool executed = false;
  {
    Thread thread;
    thread.Start([&executed]() -> void { executed = true; });
    // No Join(): the destructor joins.
  }
  ASSERT_TRUE(executed);
}

TEST(Thread, MoveAfterStart) {
  std::atomic<bool> executed{false};
  Thread first;
  first.Start([&executed]() -> void {
    executed.store(true, std::memory_order_relaxed);
  });
  // Moving a running Thread is safe: the child reports through the shared
  // state, never through `this`.
  Thread second(std::move(first));
  second.Join();
  ASSERT_TRUE(executed.load(std::memory_order_relaxed));
  ASSERT_EQ(second.GetState(), Thread::kFinished);
}

TEST(Thread, JoinWithoutStart) {
  Thread thread;
  ASSERT_DEATH(thread.Join(), "");
}

TEST(Thread, DoubleStart) {
  ASSERT_DEATH(
      {
        Thread thread;
        thread.Start([]() -> void {});
        thread.Start([]() -> void {});
      },
      "");
}

TEST(CurrentThread, IsMainThread) {
  ASSERT_TRUE(CurrentThread::IsMainThread());
  Thread thread;
  thread.Start([]() -> void { ASSERT_FALSE(CurrentThread::IsMainThread()); });
  thread.Join();
}

#if CODEBASE_IS_LINUX
TEST(CurrentThread, GetId) {
  ASSERT_EQ(CurrentThread::GetId(), ::gettid());
}
#endif

TEST(CurrentThread, IdDiffersAcrossThreads) {
  auto const main_tid = CurrentThread::GetId();
  Thread thread;
  thread.Start([main_tid]() -> void {
    ASSERT_NE(CurrentThread::GetId(), main_tid);
  });
  thread.Join();
}

TEST(CurrentThread, SleepFor) {
  ASSERT_TRUE(CurrentThread::IsMainThread());
  CurrentThread::SleepFor(std::chrono::seconds{5});
}

TEST(CurrentThread, SleepUntil) {
  ASSERT_TRUE(CurrentThread::IsMainThread());
  CurrentThread::SleepUntil(std::chrono::system_clock::now() +
                            std::chrono::seconds{5});
}

TEST(std__function, rvalue) {
  std::function<int()> function;
  {
    //
    function = std::bind([](int a, int b) -> int { return a + b; }, 4, 5);
  }
  function();
}

TEST(std__function, xvalue) {
  std::function<int()> function;
  {
    int a = 1;
    int b = 2;
    function = std::bind([](int a, int b) -> int { return a + b; },
                         std::move(a), std::move(b));
  }
  function();
}

TEST(std__function, lvalue) {
  std::function<int()> function;
  {
    int a = 1;
    int b = 2;
    function = std::bind([](int a, int b) -> int { return a + b; }, a, b);
  }
  function();
}
}  // namespace base