/**
 * @file platform_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "base/platform/platform.h"

#include <pthread.h>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <thread>

#include <gtest/gtest.h>

namespace base {

// TEST(GetPlatformProcessHandle, fork) {
//   pid_t pid_0 = GetPlatformProcessHandle();
//   pid_t pid_1 = getpid();

//   ASSERT_EQ(pid_0, pid_1);

//   pid_t child_pid;
//   if ((child_pid = fork()) == -1) {
//     std::abort();
//   } else if (child_pid == 0) {
//     pid_t pid_2 = GetPlatformProcessHandle();
//     pid_t pid_3 = getpid();
//     ASSERT_EQ(pid_2, pid_3);
//   } else {
//     pid_t pid_2 = GetPlatformProcessHandle();
//     pid_t pid_3 = getpid();
//     ASSERT_EQ(pid_2, pid_3);
//     waitpid(child_pid, nullptr, 0);
//   }
// }

TEST(GetPlatformProcessHandle, Thread) {
  std::thread thread([]() -> void {
    std::cout << "pthread id" << pthread_self() << std::endl;
    std::cout << "process id = " << GetPlatformProcessHandle() << std::endl;
    std::cout << "kthread id = " << GetPlatformThreadHandle() << std::endl;
  });
  std::cout << "pthread id" << pthread_self() << std::endl;
  std::cout << "process id = " << GetPlatformProcessHandle() << std::endl;
  std::cout << "kthread id = " << GetPlatformThreadHandle() << std::endl;
  thread.join();
}

}  // namespace base