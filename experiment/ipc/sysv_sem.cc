/**
 * @file sysv_sem.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <sched.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>

#include "experiment/helper.h"

constexpr key_t const kKey = 11236813;
constexpr int const kMode = 00755;

union semun {
  int val;
  struct semid_ds* buf;
  unsigned short* array;
#if defined(__linux__)
  struct seminfo* __buf;
#endif
};

int Server(int semid, sembuf* sops, int loop) {
  for (int i = 0; i < loop; ++i) {
    if (semop(semid, &sops[1], 1)) {
      LOG_ERR_MSG();
      std::exit(EXIT_FAILURE);
    }
    std::cerr << "Server" << std::endl;
    if (semop(semid, &sops[2], 1)) {
      LOG_ERR_MSG();
      std::exit(EXIT_FAILURE);
    }
  }
  return 0;
}

int Client(int semid, sembuf* sops, int loop) {
  for (int i = 0; i < loop; ++i) {
    if (semop(semid, &sops[3], 1) == -1) {
      LOG_ERR_MSG();
      std::exit(EXIT_FAILURE);
    }
    std::cerr << "Client" << std::endl;
    if (semop(semid, &sops[0], 1) == -1) {
      LOG_ERR_MSG();
      std::exit(EXIT_FAILURE);
    }
  }
  return 0;
}

int main() {
  int semid;

  while ((semid = semget(kKey, 2, IPC_CREAT | IPC_EXCL | kMode)) == -1) {
    if (errno == EEXIST) {
      if ((semid = semget(kKey, 2, 0)) == -1) {
        LOG_ERR_MSG();
        std::exit(EXIT_FAILURE);
      }

      if ((semctl(semid, 0, IPC_RMID)) == -1) {
        LOG_ERR_MSG();
        std::exit(EXIT_FAILURE);
      }

      std::cout << "Server removed old semaphore, id = " << semid << std::endl;
    } else {
      LOG_ERR_MSG();
      std::exit(EXIT_FAILURE);
    }
  }

  setbuf(stderr, nullptr);
  setbuf(stdout, nullptr);

  if (semctl(semid, 0, SETVAL, 1) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  if (semctl(semid, 1, SETVAL, 0) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  static sembuf sops[4];
  sops[0].sem_num = 0;
  sops[0].sem_op = 1;

  sops[1].sem_num = 0;
  sops[1].sem_op = -1;

  sops[2].sem_num = 1;
  sops[2].sem_op = 1;

  sops[3].sem_num = 1;
  sops[3].sem_op = -1;

  pid_t child_pid;
  if ((child_pid = fork()) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  } else if (child_pid == 0) {
    Server(semid, sops, 100);
  } else {
    Client(semid, sops, 100);
    waitpid(child_pid, nullptr, 0);
    semctl(semid, 0, IPC_RMID);
  }
}
