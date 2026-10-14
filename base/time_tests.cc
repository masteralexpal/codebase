/**
 * @file time_tests.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-07-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <gtest/gtest.h>
#include <time.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <ostream>
#include "base/concurrent/thread.h"

namespace base {
/**
 * @brief Tests of time(2)
 *        time(2) returns the time tick in seconds since the
 *        UNIX Epoch (1970-01-01 00:00:00 +0000 (UTC)).
 *        On error, (time_t -1) will be returned and errno
 *        will be set to indicate the error.
 *
 */
TEST(Time, time0) {
  // time_t is the alias of long int
  time_t tt;
  auto result = time(&tt);
  std::cout << tt << std::endl;
  std::cout << result << std::endl;
  ASSERT_EQ(result, tt);
}

TEST(Time, time1) {
  // time_t is the alias of long int
  // Nullable
  std::cout << time(nullptr) << std::endl;
}
/**
 * @brief Tests of ctime(2)
 *        converts the calendar time t into a null-terminated string of the
 *        form: "Wed Jun 30 21:49:08 1993\n".
 *        The call ctime(t) is equivalent to asctime(localtime(t)).
 */
TEST(Time, ctime0) {
  time_t tt;
  time(&tt);
  char const* time_str = ctime(&tt);
  std::cout << tt << std::endl;
  std::cout << time_str << std::endl;
}

// TEST(Time, ctime1) {
//   // Nullable?
//   char const *time_str = ctime(nullptr);
//   std::cout << time_str << std::endl;
// }

TEST(Time, asctime0) {
  time_t tt;
  time(&tt);
  char const* time_str = asctime(localtime(&tt));
  std::cout << tt << std::endl;
  std::cout << time_str << std::endl;
}

TEST(Time, asctime1) {
  time_t tt;
  time(&tt);
  char const* time_str = ctime(&tt);
  std::cout << tt << std::endl;
  std::cout << time_str << std::endl;
}

/**
 * @brief clock - determine processor time
 *        Time used by the program so far (user time + system time).
 *        The result / CLOCKS_PER_SECOND is program time in seconds.
 *
 */
TEST(Time, clock) {
  clock_t start = clock();
  CurrentThread::SleepFor(std::chrono::seconds{2});
  clock_t end = clock();
  long seconds = (end - start) / CLOCKS_PER_SEC;
  ASSERT_EQ(0, seconds);
}

/**
 * @brief sleep.
 *        suspend the execution of process. Linux implements sleep
 *        on the top of nanosleep.
 *
 *
 */
TEST(Time, sleep) {
  sleep(1);
}

/**
 * @brief POSIX Clock API
 *
 */

TEST(POSIX_Clock, clock_gettime) {}

}  // namespace base