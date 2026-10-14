/**
 * @file unix_socket.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <sys/socket.h>
#include <chrono>

#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <thread>

#include "experiment/helper.h"

constexpr int const kBufferSize = 40;
int Server(int unix_socket_fd) {
  static char buffer[kBufferSize];
  Zero(buffer);
  int ret;

  while ((ret = read(unix_socket_fd, buffer, kBufferSize - 1))) {
    if (ret == -1) {
      LOG_ERR_MSG();
      std::exit(errno);
    }
    buffer[ret] = 0;
    std::cout << "Server received: " << buffer << std::endl;
  }

  close(unix_socket_fd);
  return 0;
}

int Client(int unix_socket_fd) {
  while (true) {
    std::string contents;
    std::cout << "Please input your message: " << std::endl;
    std::cin >> contents;
    if (write(unix_socket_fd, contents.data(), contents.size()) == -1) {
      LOG_ERR_MSG();
      std::exit(errno);
    }
    std::cout << "Client sending finihsed!" << std::endl;
  }

  close(unix_socket_fd);
  return 0;
}

int main() {
  int unix_socket_fds[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, unix_socket_fds) == -1) {
    LOG_ERR_MSG();
    std::exit(errno);
  }

  pid_t child_pid;
  if ((child_pid = fork()) == 0) {
    close(unix_socket_fds[1]);
    Server(unix_socket_fds[0]);
  } else {
    close(unix_socket_fds[0]);
    Client(unix_socket_fds[1]);
    waitpid(child_pid, nullptr, 0);
  }
}

// #include <stdio.h>
// #include <sys/socket.h>
// #include <sys/types.h>

// #define DATA1 "In Xanadu, did Kublai Khan . . ."
// #define DATA2 "A stately pleasure dome decree . . ."

// /*
//  * This program creates a pair of connected sockets, then forks and
//  * communicates over them.  This is very similar to communication with pipes;
//  * however, socketpairs are two-way communications objects. Therefore,
//  * this program can send messages in both directions.
//  */

// int main() {
//   int sockets[2], child;
//   char buf[1024];

//   if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
//     perror("opening stream socket pair");
//     exit(1);
//   }

//   if ((child = fork()) == -1)
//     perror("fork");
//   else if (child) { /* This is the parent. */
//     close(sockets[0]);
//     if (read(sockets[1], buf, 1024) < 0)
//       perror("reading stream message");
//     printf("-->%s\n", buf);
//     if (write(sockets[1], DATA2, sizeof(DATA2)) < 0)
//       perror("writing stream message");
//     close(sockets[1]);
//   } else { /* This is the child. */
//     close(sockets[1]);
//     if (write(sockets[0], DATA1, sizeof(DATA1)) < 0)
//       perror("writing stream message");
//     if (read(sockets[0], buf, 1024) < 0)
//       perror("reading stream message");
//     printf("-->%s\n", buf);
//     close(sockets[0]);
//   }
// }