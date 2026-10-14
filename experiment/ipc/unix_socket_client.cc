/**
 * @file unix_socket_client.cc
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
#include <cstdlib>
#include <cstring>

#include "experiment/helper.h"

constexpr char unix_socket_name[] = "/tmp/unix-socket.lpa";

int Clinet() {
  sockaddr_un client_socket_address;
  int socket_fd;

  client_socket_address.sun_family = AF_LOCAL;
  strcpy(client_socket_address.sun_path, unix_socket_name);

  if ((socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  if ((connect(socket_fd, reinterpret_cast<sockaddr*>(&client_socket_address),
               sizeof(client_socket_address))) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  constexpr int const kBufferSize = 4096;
  static char buffer[kBufferSize];
  while (true) {
    std::cout << "Plase input your message: " << std::endl;
    std::cin >> buffer;

    if ((write(socket_fd, buffer, strlen(buffer))) == -1) {
      LOG_ERR_MSG();
      std::exit(EXIT_FAILURE);
    }

    std::cout << "Message: " << buffer << " sending finished" << std::endl;
  }

  close(socket_fd);
  return 0;
}

int main() {
  std::exit(Clinet());
  return 0;
}