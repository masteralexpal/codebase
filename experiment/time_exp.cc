/**
 * @file time_exp.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <sys/time.h>
#include <sys/timerfd.h>

#include <gtest/gtest.h>
#include "base/log.h"

class TimeTestEnviroment : public testing::Environment {
 public:
  void SetUp() override {
    base::log::LogConfiguration config;
    config.SetMinLogSeverity(base::log::LOG_VERBOSE);
    base::log::Initialize(config);
    LOG(INFO) << __func__ << "() called!";
  }
};

class CalendarTest : public testing::Test {
 public:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(CalendarTest, time) {
  time_t t0;
  time_t t1 = time(&t0);
  ASSERT_EQ(t1, t0);
  LOG(INFO) << "t0 = " << t0 << ", t1 = " << t1;
}

TEST_F(CalendarTest, ctime) {
  time_t t = time(nullptr);
  LOG(INFO) << ctime(&t);
}

// gettimeofday is deprecated.
TEST_F(CalendarTest, gettimeofday) {
  timeval tv;
  gettimeofday(&tv, nullptr);
  LOG(INFO) << tv.tv_sec << std::endl;
  LOG(INFO) << tv.tv_usec << std::endl;
}

// gettimeofday is deprecated.
TEST_F(CalendarTest, gmtime) {
  time_t tt = time(nullptr);
  tm* t = gmtime(&tt);
  ASSERT_NE(t, nullptr);
}

TEST_F(CalendarTest, localtime) {
  time_t tt = time(nullptr);
  tm* t = localtime(&tt);
  ASSERT_NE(t, nullptr);
}

TEST_F(CalendarTest, mktime) {
  time_t tt = time(nullptr);
  tm* t = localtime(&tt);
  time_t ttt = mktime(t);
  ASSERT_NE(t, nullptr);
  ASSERT_EQ(ttt, tt);
}

TEST_F(CalendarTest, asctime) {
  time_t tt = time(nullptr);
  LOG(INFO) << asctime(localtime(&tt));
  LOG(INFO) << asctime(gmtime(&tt));
}

class ClockTest : public testing::Test {
 public:
  void SetUp() override {}

  void TearDown() override {}

 private:
};

TEST_F(ClockTest, GetResolution) {
  timespec ts;
  clock_getres(CLOCK_REALTIME, &ts);
  LOG(INFO) << "CLOCK_REALTIME, tv.tv_sec = " << ts.tv_sec
            << ", ts.tv_nsec = " << ts.tv_nsec;
  clock_getres(CLOCK_BOOTTIME, &ts);
  LOG(INFO) << "CLOCK_BOOTTIME, tv.tv_sec = " << ts.tv_sec
            << ", ts.tv_nsec = " << ts.tv_nsec;
  clock_getres(CLOCK_MONOTONIC, &ts);
  LOG(INFO) << "CLOCK_MONOTONIC, tv.tv_sec = " << ts.tv_sec
            << ", ts.tv_nsec = " << ts.tv_nsec;
}

#if defined(CLOCK_REALTIME)
TEST_F(ClockTest, GetRealTime) {
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  std::cout << "real time seconds: " << ts.tv_sec
            << ", nanoseconds = " << ts.tv_nsec << std::endl;
  std::cout << ctime(&ts.tv_sec);
  static char buffer[1024];
  std::string time_fmt = "[%04d-%02d-%02d %02d:%02d:%02d.%d]";
  tm* ptm = localtime(&ts.tv_sec);
  std::sprintf(buffer, time_fmt.c_str(), ptm->tm_year + 1900, ptm->tm_mon + 1,
               ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec,
               ts.tv_nsec);
  std::cout << buffer << std::endl;
}
#endif

#if defined(CLOCK_MONOTONIC)
TEST_F(ClockTest, GetMonotonicTime) {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  std::cout << "monotonic time seconds: " << ts.tv_sec
            << ", nanoseconds = " << ts.tv_nsec << std::endl;
}
#endif

#if defined(CLOCK_BOOTTIME)
TEST_F(ClockTest, GetBootTime) {
  timespec ts;
  clock_gettime(CLOCK_BOOTTIME, &ts);
  std::cout << "boot time seconds: " << ts.tv_sec
            << ", nanoseconds = " << ts.tv_nsec << std::endl;
}
#endif

class TimerTest : public testing::Test {
 public:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(TimerTest, Case) {}

class TimerfdTest : public testing::Test {
 public:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(TimerfdTest, Create) {
  int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
  ASSERT_NE(timer_fd, -1);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::AddGlobalTestEnvironment(new TimeTestEnviroment{});
  return RUN_ALL_TESTS();
}