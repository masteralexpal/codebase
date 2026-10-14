/**
 * @file mmap.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "errno.h"
#include "fcntl.h"
#include "unistd.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

int main() {
  int fd;
  struct stat sb;
  char* mapped;

  // 打开文件
  mode_t mode = 0755;
  fd = open("example.txt", O_RDWR, mode);
  if (fd == -1) {
    perror("open");
    exit(1);
  }

  // 获取文件大小
  if (fstat(fd, &sb) == -1) {
    perror("fstat");
    exit(1);
  }

  // 将文件映射到内存
  mapped = static_cast<char*>(
      mmap(nullptr, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (mapped == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  // 修改映射的内存
  //   mapped[0] = 'H';
  //   mapped[1] = 'e';
  //   mapped[2] = 'l';
  //   mapped[3] = 'l';
  //   mapped[4] = 'o';
  for (auto i = 0; i < sb.st_size; ++i) {
    std::cout << mapped[i];
  }

  // 解除映射
  if (munmap(mapped, sb.st_size) == -1) {
    perror("munmap");
    exit(1);
  }

  // 关闭文件
  close(fd);
  return 0;
}