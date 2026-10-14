/**
 * @file posix_sem.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdio>
#include <random>
#include <thread>

#include <cerrno>
#include <cstdlib>

#include "experiment/helper.h"

// CAUTION! The name of a semaphore can have ONLY one splash!
// Ref: https://www.man7.org/linux/man-pages/man7/sem_overview.7.html
char const semaphore_server[] = "/semaphore.server";
char const semaphore_client[] = "/semaphore.client";

int Server(sem_t* sem_server, sem_t* sem_client) {
  while (true) {
    // std::this_thread::sleep_for(std::chrono::seconds{1});
    sem_wait(sem_server);
    std::cout << "Server: do something ..." << std::endl;
    sem_post(sem_client);
  }
  return 0;
}

int Client(sem_t* sem_client, sem_t* sem_server) {
  while (true) {
    // std::this_thread::sleep_for(std::chrono::seconds{1});
    sem_wait(sem_client);
    std::cout << "Client: do something ..." << std::endl;
    sem_post(sem_server);
  }
  return 0;
}

int main() {
  int flag = O_CREAT | O_RDWR | O_TRUNC;
  mode_t mode = 0755;
  unlink(semaphore_client);
  unlink(semaphore_server);
  sem_t* sem_server = sem_open(semaphore_server, flag, mode, 1);
  if (!sem_server) {
    LOG_ERR_MSG();
    return errno;
  }
  sem_t* sem_client = sem_open(semaphore_client, flag, mode, 1);
  if (!sem_client) {
    LOG_ERR_MSG();
    return errno;
  }
  setbuf(stdout, nullptr);
  // sem_init(semaphore, false, 1);
  pid_t child_pid = fork();
  if (child_pid == -1) {
    LOG_ERR_MSG();
    std::exit(errno);
  } else if (child_pid == 0) {
    Server(sem_server, sem_client);
  } else {
    Client(sem_client, sem_server);
    waitpid(child_pid, nullptr, 0);
    return 0;
  }
}