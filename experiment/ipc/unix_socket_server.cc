/**
 * @file unix_socket_server.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cerrno>
#include <ostream>

#include <cstdlib>
#include <cstring>

#include "experiment/helper.h"

constexpr char unix_socket_name[] = "/tmp/unix-socket.lpa";

int ServerMain() {
  sockaddr_un server_socket;
  int listen_fd;

  server_socket.sun_family = AF_LOCAL;
  strcpy(server_socket.sun_path, unix_socket_name);

  // Create a new unix socket fd.
  if ((listen_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  if (bind(listen_fd, reinterpret_cast<sockaddr*>(&server_socket),
           sizeof(server_socket)) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  if ((listen(listen_fd, 100)) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  std::cout << "Server: create socket fd = " << listen_fd << std::endl;

  constexpr int const kBufferSize = 8;
  static char buffer[kBufferSize];
  int ret;

  while (true) {
    int connect_fd = accept(listen_fd, nullptr, nullptr);
    while ((ret = read(connect_fd, buffer, kBufferSize - 1))) {
      if (ret == -1) {
        if (errno == EAGAIN) {
          continue;
        } else {
          LOG_ERR_MSG();
          std::exit(EXIT_FAILURE);
        }
      }
      buffer[ret] = 0;
      std::cout << "Message: " << buffer << std::endl;
    }
    std::cout << "Server received EOF" << std::endl;
    close(connect_fd);
  }

  close(listen_fd);
  close(listen_fd);
  return 0;
}

int main() {
  unlink(unix_socket_name);
  std::exit(ServerMain());
  return 0;
}