/**
 * @file unix_socket_multiplex_server.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cerrno>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>

#include "experiment/helper.h"

constexpr char const kUnixSocketName[] = "/tmp/unix-socket.lpa";
constexpr int const kMaxUnixSocketPathLength = 104;
constexpr int const kMaxEpollEvents = 1024;
constexpr int const kBufferSize = 4096;

class IOTaskQueue {
 public:
  IOTaskQueue() = default;

  virtual ~IOTaskQueue() = default;

  void Enqueue(int fd) {
    std::cout << "Enqueue fd " << fd << std::endl;
    {
      std::lock_guard<std::mutex> lock_guard(queue_lock_);
      fds_queue_.push_back(fd);
    }
    condition_variable_.notify_all();
  }

  int Dequeue() {
    int fd;
    {
      std::unique_lock<std::mutex> unique_lock(queue_lock_);
      while (fds_queue_.empty()) {
        condition_variable_.wait(unique_lock);
      }
      fd = fds_queue_.front();
      fds_queue_.pop_front();
    }
    std::cout << "Dequeue fd " << fd << std::endl;
    return fd;
  }

  std::size_t Size() const noexcept { return fds_queue_.size(); }

  bool Empty() const noexcept { return fds_queue_.empty(); }

 private:
  std::mutex queue_lock_;
  std::condition_variable condition_variable_;
  std::deque<int> fds_queue_;
};

void IOThreadTask(IOTaskQueue* io_task_queue) {
  std::cout << "Startd IO Thread" << std::endl;
  static char buffer[kBufferSize];
  int nbytes;
  while (true) {
    int fd = io_task_queue->Dequeue();
    std::cerr << "IOThread start reading fd = " << fd << std::endl;
    while ((nbytes = read(fd, buffer, kBufferSize - 1))) {
      if (nbytes == -1) {
        if (errno == EAGAIN) {
          break;
        } else {
          LOG_ERR_MSG();
          std::exit(EXIT_FAILURE);
        }
      } else {
        buffer[nbytes] = 0;
        std::cout << buffer;
      }
    }
    std::cout << std::endl;
  }
}

int ServerMain() {
  sockaddr_un socket_address;
  int listen_fd;
  int connect_fd;
  int epoll_fd;
  // int nbytes;
  epoll_event current_event;
  epoll_event events[kMaxEpollEvents];
  // static char buffer[kBufferSize];
  IOTaskQueue io_task_queue;
  std::thread io_thread(IOThreadTask, &io_task_queue);

  socket_address.sun_family = AF_UNIX;
  strncpy(socket_address.sun_path, kUnixSocketName, kMaxUnixSocketPathLength);

  if ((listen_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  if ((bind(listen_fd, reinterpret_cast<sockaddr*>(&socket_address),
            sizeof(socket_address))) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  if (listen(listen_fd, 100) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  if ((epoll_fd = epoll_create(1)) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  current_event.data.fd = listen_fd;
  current_event.events = EPOLLIN;

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &current_event) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  while (true) {
    int nfds = epoll_wait(epoll_fd, events, kMaxEpollEvents, -1);

    if (nfds == -1) {
      LOG_ERR_MSG();
      std::exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == listen_fd) {
        std::cerr << "Server received a new connection." << std::endl;
        if ((connect_fd = accept4(listen_fd, nullptr, nullptr,
                                  SOCK_NONBLOCK | SOCK_CLOEXEC)) == -1) {
          LOG_ERR_MSG();
          std::exit(EXIT_FAILURE);
        }
        current_event.data.fd = connect_fd;
        current_event.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;
        if ((epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connect_fd, &current_event)) ==
            -1) {
          LOG_ERR_MSG();
          std::exit(EXIT_FAILURE);
        }
      } else {
        std::cerr << "Server received client's data" << std::endl;
        if (events[i].events == EPOLLIN) {
          io_task_queue.Enqueue(events[i].data.fd);
        }
      }
    }
  }
  io_thread.join();
  return 0;
}

int main() {
  if (unlink(kUnixSocketName) == -1) {
    if (errno != ENOENT) {
      LOG_ERR_MSG();
      std::exit(EXIT_FAILURE);
    }
  }

  return ServerMain();
}