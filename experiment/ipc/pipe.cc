/**
 * @file pipe.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>

#include "experiment/helper.h"

//
int main() {
  int pipe_0[2];
  int pipe_1[2];
  pid_t child_pid;
  if ((pipe(pipe_0)) == -1) {
    LOG_ERR_MSG();
  }
  if ((pipe(pipe_1)) == -1) {
    LOG_ERR_MSG();
  }

  if ((child_pid = fork()) == 0) {
    // Child process.
    close(pipe_0[1]);
    close(pipe_1[0]);

    constexpr int const buffer_size = 4096;
    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);

    int ret = 0;
    while (true) {
      ret = read(pipe_0[0], buffer, buffer_size - 1);
      if (ret == -1) {
        LOG_ERR_MSG();
        std::exit(errno);
      }

      buffer[ret] = '\0';
      std::cout << "Child Process Recevied: " << buffer << std::endl;
      std::cout << "Child Process Send Message Back To Parent Process."
                << std::endl;

      write(pipe_1[1], buffer, ret);
      for (auto&& c : buffer) {
        if (c == '\0')
          break;
        if (c == 'Q')
          std::exit(0);
      }
    }
  } else {
    close(pipe_0[0]);
    close(pipe_1[1]);

    constexpr int const buffer_size = 4096;
    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);

    int ret;
    while (true) {
      std::cout << "Please input your message:" << std::endl;
      std::scanf("%4095s", buffer);
      std::cout << "Parent Process Send Message To Child Procss." << std::endl;

      write(pipe_0[1], buffer, strlen(buffer));
      ret = read(pipe_1[0], buffer, buffer_size - 1);
      if (ret == -1) {
        LOG_ERR_MSG();
        std::exit(errno);
      }
      buffer[ret] = 0;
      std::cout << "Parental Process Recevied: " << buffer << std::endl;
    }
    waitpid(child_pid, nullptr, 0);
  }
}
