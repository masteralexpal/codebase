/**
 * @file posix_shm.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstdlib>

#include "experiment/helper.h"

char const shared_memory_name[] = "/shm.lpa";

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
  shm_unlink(shared_memory_name);
  int oflag = O_RDWR | O_CREAT | O_TRUNC;
  mode_t mode = 0755;
  int mmap_fd;
  if ((mmap_fd = shm_open(shared_memory_name, oflag, mode)) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  // or: write(mmap_fd, &count_block, sizeof(count_block));
  ftruncate(mmap_fd, sizeof(CountBlock));

  CountBlock* cb = static_cast<CountBlock*>(mmap(nullptr, sizeof(CountBlock),
                                                 PROT_READ | PROT_WRITE,
                                                 MAP_SHARED, mmap_fd, 0));
  cb->count = 0;
  pthread_mutexattr_t mutex_attr;
  pthread_mutexattr_init(&mutex_attr);
  // This is the point.
  // https://www.gonwan.com/2014/04/10/sharing-mutex-and-condition-variable-between-processes/
  pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(cb->mutex), &mutex_attr);
  pthread_mutexattr_destroy(&mutex_attr);

  if (!cb) {
    LOG_ERR_MSG();
  }

  int loop = 100'000'000;
  pid_t child_pid = fork();
  if (child_pid == -1) {
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

    munmap(cb, sizeof(CountBlock));
    shm_unlink(shared_memory_name);
    return EXIT_SUCCESS;
  }
}