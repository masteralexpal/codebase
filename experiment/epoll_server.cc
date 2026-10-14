/**
 * @file epoll_server.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-02
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cerrno>
#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "base/command_line.h"

static std::size_t constexpr kBufferSize = 4096;
static in_port_t constexpr kPort = 15672;
static std::size_t constexpr kMaxEvents = 128;

int main(int argc, char* argv[]) {
  base::CommandLine::Initialize(argc, argv);

  sockaddr_in server_addr = {};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(kPort);
  auto port = base::CommandLine::Get()->GetValue("port");
  if (port) {
    server_addr.sin_port = htons(std::atoi(port->c_str()));
  }
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd == -1) {
    std::cerr << "socket failed. Error: " << std::strerror(errno) << std::endl;
    return errno;
  }

  if (bind(listen_fd, reinterpret_cast<sockaddr*>(&server_addr),
           sizeof(server_addr)) == -1) {
    std::cerr << "bind failed. Error: " << std::strerror(errno) << std::endl;
  }

  if (listen(listen_fd, 128)) {
    std::cerr << "listen failed. Error: " << std::strerror(errno) << std::endl;
  }

  int epoll_fd = epoll_create(1);
  if (epoll_fd == -1) {
    std::cerr << "epoll_create failed. Error: " << std::strerror(errno)
              << std::endl;
  }

  epoll_event event;
  epoll_event events[kMaxEvents];
  event.data.fd = listen_fd;
  event.events = EPOLLIN;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event) == -1) {
    std::cerr << "epoll_ctl failed. Error: " << std::strerror(errno)
              << std::endl;
    return errno;
  }

  char buffer[kBufferSize] = {};

  while (true) {
    std::cerr << "Entering epoll_wait." << std::endl;
    int nfds = epoll_wait(epoll_fd, events, kMaxEvents, -1);
    std::cerr << "Wake from epoll_wait." << std::endl;
    if (nfds == -1) {
      std::cerr << "epoll_wait failed. Error: " << std::strerror(errno)
                << std::endl;
      return errno;
    }
    for (int i = 0; i < nfds; ++i) {
      std::cout << "fd = " << events[i].data.fd << std::endl;
      if (events[i].data.fd == listen_fd) {
        std::cerr << "On new connection received." << std::endl;
        int connect_fd =
            accept4(listen_fd, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (connect_fd == -1) {
          std::cerr << "accept4 failed. Error: " << std::strerror(errno)
                    << std::endl;
          return errno;
        }
        event.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;
        event.data.fd = connect_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connect_fd, &event) == -1) {
          std::cerr << "epoll_ctl failed. Error: " << std::strerror(errno)
                    << std::endl;
          return errno;
        }
      } else {
        std::cerr << "On new client data received." << std::endl;
        int ret = -1;
        std::cout << "Recevied: ";
        while ((ret = read(events[i].data.fd, buffer, kBufferSize - 1))) {
          if (ret == -1) {
            if (errno == EAGAIN) {
              continue;
            }
            std::cerr << "read failed. Error: " << std::strerror(errno)
                      << std::endl;
            return errno;
          }
          buffer[ret] = '\0';
          std::cout << buffer;
        }
        std::cout << std::endl;
      }
    }
  }
  return 0;
}
