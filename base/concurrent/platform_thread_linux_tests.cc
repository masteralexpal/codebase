/**
 * @file platform_thread_linux_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "base/concurrent/platform_thread_linux.h"

#include <syscall.h>
#include <unistd.h>

#include <chrono>
#include <memory>
#include <ostream>
#include <thread>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>
#include "base/log.h"

class DeleagteImpl : public base::PlatformThread::Delegate {
 public:
  template <typename ForwardFunctor, typename... ForwardArguments>
  DeleagteImpl(ForwardFunctor&& f, ForwardArguments&&... args)
      : closure_([f = std::forward<ForwardFunctor>(f),
                  args_ = std::make_tuple(
                      std::forward<ForwardArguments>(args)...)]() -> void {
          LOG(INFO) << __func__ << "() called!" << std::endl;
          std::apply(f, args_);
        }) {}

  virtual void ThreadMain() override {
    LOG(INFO) << __func__ << "() called!" << std::endl;
    closure_();
  }

 private:
  std::function<void()> closure_;
};

void TestThreadName(std::string const& name) {
  base::log::LogConfiguration config;
  config.SetMinLogSeverity(base::log::LOG_INFO);
  base::log::Initialize(config);

  base::PlatformThread::Handle handle =
      base::PlatformThread::Current::GetHandle();

  ASSERT_EQ(base::PlatformThread::SetName(handle, name), 0);

  std::string thread_name;
  base::PlatformThread::GetName(handle, thread_name);

  ASSERT_EQ(thread_name, name);
}

void TestThreadHandle() {
  base::PlatformThread::Handle pthread_id =
      base::PlatformThread::Current::GetHandle();

  base::PlatformThread::KernelHandle lwp_id =
      base::PlatformThread::Current::GetKernelHandle();

  ASSERT_EQ(pthread_id, pthread_self());
  ASSERT_EQ(lwp_id, syscall(SYS_gettid));
}

TEST(PlatformThreadLinux, SetAndGetThreadName) {
  TestThreadName("Main");
  std::thread thread(TestThreadName, "Child");
  thread.join();
}

TEST(PlatformThreadLinux, Handle) {
  TestThreadHandle();
  std::thread thread(TestThreadHandle);
  thread.join();
}

TEST(PlatformThreadLinux, IsMain) {
  ASSERT_TRUE(base::PlatformThread::Current::IsMainThread());
  std::thread thread([]() -> void {
    ASSERT_FALSE(base::PlatformThread::Current::IsMainThread());
  });
  thread.join();
}

void add(int a, int b) {
  LOG(INFO) << __func__ << "() called!" << std::endl;
  std::ignore = a + b;
}

TEST(PlatformThreadLinux, Spawn) {
  base::PlatformThread::Handle handle;
  auto delegate = std::make_unique<DeleagteImpl>(add, 1, 2);

  base::PlatformThread::Spawn(
      static_cast<base::PlatformThread::Delegate*>(delegate.get()), &handle);

  base::PlatformThread::Join(handle);
}

TEST(PlatformThreadLinux, Yield) {
  base::PlatformThread::Current::Yield();
}

TEST(PlatformThreadLinux, SleepFor) {
  base::PlatformThread::Current::SleepFor(std::chrono::seconds{1});
}

TEST(PlatformThreadLinux, SleepUntil) {
  base::PlatformThread::Current::SleepUntil(std::chrono::seconds{1} +
                                            std::chrono::system_clock::now());
}

class DeleagteImpl2 : public base::PlatformThread::Delegate {
 public:
  void ThreadMain() override {
    LOG(INFO) << "This = " << this;
    LOG(INFO) << "tls_delegate = "
              << base::PlatformThread::Current::GetDelegate();
  }
};
TEST(PlatformThreadLinux, GetDelegate) {
  DeleagteImpl2 d1;
  DeleagteImpl2 d2;

  base::PlatformThread::Handle h1;
  base::PlatformThread::Handle h2;

  base::PlatformThread::Spawn(&d1, &h1);
  base::PlatformThread::Spawn(&d2, &h2);

  base::PlatformThread::Join(h1);
  base::PlatformThread::Join(h2);

  ASSERT_EQ(base::PlatformThread::Current::GetDelegate(), nullptr);
}
