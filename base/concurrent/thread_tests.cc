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
#include <chrono>

namespace base {
TEST(Thread, Creation) {
  Thread thread;
  ASSERT_STREQ(thread.GetName().c_str(), "Anonymous");
  ASSERT_EQ(thread.GetSate(), Thread::kCreated);
}
void PlainFunction0() { std::cout << __func__ << "() called!"; }

TEST(Thread, StartWithPlainFunction0) {
  Thread thread("Main");
  ASSERT_EQ(thread.GetSate(), Thread::kCreated);
  ASSERT_STREQ(thread.GetName().c_str(), "Main");
  thread.Start(PlainFunction0);
  ASSERT_EQ(thread.GetSate(), Thread::kStarted);
  thread.Join();
}

void PlainFunction1(int a, int b, int *c) { *c = a + b; }

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
  void operator()() {}
};

TEST(Thread, StartWithFunctor) {}

TEST(CurrnetThread, IsMainThread) {
  ASSERT_TRUE(CurrentThread::IsMainThread());
  Thread thread;
  thread.Start([]() -> void { ASSERT_FALSE(CurrentThread::IsMainThread()); });
  thread.Join();
}

TEST(CurrnetThread, SleepFor) {
  ASSERT_TRUE(CurrentThread::IsMainThread());
  CurrentThread::SleepFor(std::chrono::seconds{5});
}

TEST(CurrnetThread, SleepUntil) {
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
} // namespace base