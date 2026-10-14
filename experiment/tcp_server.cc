/**
 * @file tcp_server.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <netinet/in.h>
#include <unistd.h>
#include <cstddef>
#include <cstring>

#include <iostream>

#include "base/command_line.h"

constexpr std::size_t kDefaultBufferSize = 4096u;
constexpr int kDefaultPort = 15672;

/**
 * @brief echo server
 *
 * @param argc
 * @param argv
 * @return int return code.
 */
int main(int argc, char** argv) {
  base::CommandLine::Initialize(argc, argv);
  int listen_fd = 0;
  int connect_fd = 0;
  uint16_t port = 0;
  sockaddr_in server_addr;
  char buffer[kDefaultBufferSize];
  //
  std::memset(&server_addr, 0, sizeof(server_addr));
  std::memset(&buffer, 0, sizeof(buffer));

  auto p = base::CommandLine::Get()->GetValue("port");
  if (!p) {
    port = kDefaultPort;
    std::cerr << "--port=[port] pased failed, using default port: "
              << kDefaultPort << std::endl;
  } else {
    port = std::atoi(p->c_str());
    std::cerr << "Using port: " << port << std::endl;
  }

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == listen_fd) {
    std::cerr << "create socket failed! " << std::strerror(errno) << std::endl;
    return errno;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  if (-1 == bind(listen_fd, reinterpret_cast<sockaddr*>(&server_addr),
                 sizeof(server_addr))) {
    std::cout << "bind error! " << std::strerror(errno) << std::endl;
  }

  if (-1 == listen(listen_fd, 5)) {
    std::cout << "listen error! " << std::strerror(errno) << std::endl;
    return errno;
  }

  int result = 0;
  while (true) {
    connect_fd = accept(listen_fd, nullptr, nullptr);
    if (-1 == connect_fd) {
      std::cerr << "accept error! " << std::strerror(errno) << std::endl;
      continue;
    }
    while ((result = read(connect_fd, buffer, kDefaultBufferSize - 1))) {
      if (-1 == result) {
        if (EAGAIN == errno) {
          continue;
        }
        std::cerr << "read error! " << std::strerror(errno) << std::endl;
      }
      buffer[result] = 0;
      std::cout << buffer << std::endl;
      write(connect_fd, buffer, result);
    }
    close(connect_fd);
  }
  return 0;
}