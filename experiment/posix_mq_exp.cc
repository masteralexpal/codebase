/**
 * @file posix_mq_exp.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <fcntl.h>
#include <mqueue.h>
#include <chrono>
#include <cstdlib>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <ostream>

static char const mq_name[] = "/tmp/mqueue.lipingan";

int main() {
  mqd_t mq;
  mq = mq_open(mq_name, O_RDWR | O_CREAT);
  if (mq == -1) {
    std::cout << std::strerror(errno) << std::endl;
    std::exit(errno);
  }
}