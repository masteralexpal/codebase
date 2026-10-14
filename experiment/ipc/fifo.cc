/**
 * @file fifo.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-03
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "experiment/helper.h"

char const ctos[] = "/tmp/ctos.fifo";
char const stoc[] = "/tmp/stoc.fifo";

void Server(int ctos_fd, int stoc_fd) {
  constexpr int const buffer_size = 4096;
  static char buffer[buffer_size];
  memset(buffer, 0, buffer_size);
  int ret = 0;
  while (true) {
    ret = read(ctos_fd, buffer, buffer_size - 1);
    if (ret == -1) {
      LOG_ERR_MSG();
      std::exit(errno);
    }

    buffer[ret] = 0;
    std::cout << "Server Recevied: " << buffer << std::endl;
    std::cout << "Server Send Back To Client." << std::endl;
    ret = write(stoc_fd, buffer, ret);
    if (ret == -1) {
      LOG_ERR_MSG();
      std::exit(errno);
    }
  }
}
void Client(int stoc_fd, int ctos_fd) {
  constexpr int const buffer_size = 4096;
  static char buffer[buffer_size];
  memset(buffer, 0, buffer_size);
  int ret = 0;
  while (true) {
    std::cout << "Please Input Your Message: " << std::endl;
    std::scanf("%s", buffer);
    std::cout << "Client Send Message To Server Through FIFO" << std::endl;
    std::cout << "Message: " << buffer << std::endl;
    ret = write(ctos_fd, buffer, strlen(buffer));
    if (ret == -1) {
      LOG_ERR_MSG();
      std::exit(errno);
    }
    ret = read(stoc_fd, buffer, buffer_size - 1);
    if (ret == -1) {
      LOG_ERR_MSG();
      std::exit(errno);
    }
    buffer[ret] = 0;
    std::cout << "Client Received: " << buffer << std::endl;
  }
}

int main() {
  mode_t mode = 0755;
  int oflag = O_RDWR;
  // mkfifo -> EESIXTS -> open
  // mkfifo does NOT return a fd!
  if (mkfifo(ctos, mode) == -1) {
    if (errno != EEXIST) {
      LOG_ERR_MSG();
      std::exit(errno);
    }
  }

  int ctos_fd;
  if ((ctos_fd = open(ctos, oflag)) == -1) {
    LOG_ERR_MSG();
    std::exit(errno);
  }

  if (mkfifo(stoc, mode) == -1) {
    if (errno != EEXIST) {
      LOG_ERR_MSG();
      std::exit(errno);
    }
  }

  int stoc_fd;
  if ((stoc_fd = open(stoc, oflag)) == -1) {
    LOG_ERR_MSG();
    std::exit(errno);
  }

  pid_t child_pid;
  if ((child_pid = fork()) == 0) {
    Server(ctos_fd, stoc_fd);
  } else {
    Client(stoc_fd, ctos_fd);
    waitpid(child_pid, nullptr, 0);
    close(ctos_fd);
    close(stoc_fd);
    // Must explicitly deleted by unlink()
    unlink(ctos);
    unlink(stoc);
  }
};
