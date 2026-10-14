/**
 * @file tcp_client.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "base/command_line.h"

constexpr std::size_t kBufferSize = 4u;
constexpr int kPort = 15672;

int main(int argc, char* argv[]) {
  base::CommandLine::Initialize(argc, argv);

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    std::cerr << "socket() failed. Error: " << std::strerror(errno)
              << std::endl;
    return errno;
  }

  sockaddr_in server_addr = {};
  server_addr.sin_family = AF_INET;
  std::string ip_addres = "127.0.0.1";
  in_port_t port = kPort;
  auto ip = base::CommandLine::Get()->GetValue("ip");
  if (ip) {
    ip_addres = ip.value();
  }

  if (inet_pton(AF_INET, ip_addres.c_str(), &server_addr.sin_addr) == -1) {
    std::cerr << "inet_pton() failed. Error: " << std::strerror(errno)
              << std::endl;
    return errno;
  }

  auto port_num = base::CommandLine::Get()->GetValue("port");
  if (port_num) {
    port = std::atoi(port_num->c_str());
  }
  server_addr.sin_port = htons(port);

  if (connect(socket_fd, reinterpret_cast<sockaddr*>(&server_addr),
              sizeof(server_addr)) == -1) {
    std::cerr << "connect() failed. Error " << std::strerror(errno)
              << std::endl;
    return errno;
  }

  char buffer[kBufferSize] = {};
  int ret = 0;
  while (true) {
    while ((ret = read(STDIN_FILENO, buffer, kBufferSize - 1))) {
      if (ret == -1) {
        if (errno == EAGAIN) {
          continue;
        }
        return errno;
      }
      buffer[ret] = '\0';
      std::cout << "send: " << buffer << std::endl;
      send(socket_fd, buffer, ret, 0);
    }
  }
  return 0;
}