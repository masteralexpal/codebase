/**
 * @file pipe_exp.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <sys/wait.h>
#include <unistd.h>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <thread>

#include "experiment/helper.h"

int main() {
  int pipe_0[2];  // pipe_0[0] used for reading and pipe_0[1] used for writing.
  int pipe_1[2];

  pid_t child_pid;

  pipe(pipe_0);
  pipe(pipe_1);

  if ((child_pid = fork()) == 0) {
    close(pipe_0[1]);
    close(pipe_1[0]);
    std::string contents = "Hello world!";
    write(pipe_1[1], contents.data(), contents.size());
  } else {
    std::thread io_thread{};
    close(pipe_0[0]);
    close(pipe_1[1]);
    char buffer[1024];
    Zero(buffer);
    int result = 0;
    while ((result = read(pipe_1[0], buffer, sizeof(buffer)))) {
      std::cout << buffer;
    }
    waitpid(child_pid, nullptr, 0);
  }
}
