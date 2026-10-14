/**
 * @file shared_memory_exp.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <cstdint>
#include <string>

struct InterProcessBlock {
  sem_t semaphore;
  std::int64_t count;
};

int main() {
  InterProcessBlock inter_process_block;
  std::string file_path = "/tmp/A87GnAmKG.mmap";
  int fd = open(file_path.c_str(), O_RDWR | O_CREAT, 0777);
  if (fd == -1) {
    std::cout << std::strerror(errno) << std::endl;
    std::exit(errno);
  }
  sem_init(&inter_process_block.semaphore, 1, 1);
  inter_process_block.count = 0;
  write(fd, &inter_process_block, sizeof(inter_process_block));
  InterProcessBlock* ipb = static_cast<InterProcessBlock*>(
      mmap(nullptr, sizeof(inter_process_block), PROT_READ | PROT_WRITE,
           MAP_SHARED, fd, 0));
  pid_t child_pid;
  if ((child_pid = fork()) == 0) {
    for (int i = 0; i < 1000000; i++) {
      sem_wait(&ipb->semaphore);
      ++ipb->count;
      sem_post(&ipb->semaphore);
    }

  } else {
    for (int i = 0; i < 1000000; i++) {
      sem_wait(&ipb->semaphore);
      ++ipb->count;
      sem_post(&ipb->semaphore);
    }
    waitpid(child_pid, nullptr, 0);
    std::cout << "count = " << ipb->count << std::endl;
  }
}