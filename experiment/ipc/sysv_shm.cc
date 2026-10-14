/**
 * @file sysv_shm.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <pthread.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>

#include "experiment/helper.h"

struct CountBlock {
  int count;
  pthread_mutex_t mutex;
};

int Server(CountBlock* count_block, int loop) {
  for (int i = 0; i < loop; ++i) {
    pthread_mutex_lock(&(count_block->mutex));
    ++(count_block->count);
    pthread_mutex_unlock(&(count_block->mutex));
  }
  return 0;
}

int Client(CountBlock* count_block, int loop) {
  for (int i = 0; i < loop; ++i) {
    pthread_mutex_lock(&(count_block->mutex));
    ++(count_block->count);
    pthread_mutex_unlock(&(count_block->mutex));
  }
  return 0;
}

int main() {
  int shmid = shmget(IPC_PRIVATE, sizeof(CountBlock), IPC_CREAT | 0755);
  if (shmid == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  CountBlock* cb = static_cast<CountBlock*>(shmat(shmid, nullptr, 0));
  if (!cb) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  cb->count = 0;
  pthread_mutexattr_t mutex_attr;
  pthread_mutexattr_init(&mutex_attr);
  pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&cb->mutex, &mutex_attr);
  pthread_mutexattr_destroy(&mutex_attr);

  int loop = 100'000;
  pid_t child_pid;
  if ((child_pid = fork()) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  } else if (child_pid == 0) {
    Server(cb, loop);
  } else {
    Client(cb, loop);
    waitpid(child_pid, nullptr, 0);
    if (cb->count != 2 * loop) {
      std::cout << "Error count = " << cb->count << ", is not equal to "
                << 2 * loop << std::endl;
    } else {
      std::cout << "Count = " << cb->count << ", the result is OK" << std::endl;
    }
  }

  return EXIT_SUCCESS;
}