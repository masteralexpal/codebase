/**
 * @file fcntl_exp.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <fcntl.h>

#include "experiment/helper.h"
#include <gtest/gtest.h>

TEST(fcntl, dupfd) {
  std::string file_path = "test_file";
  int flag = O_RDWR | O_CREAT | O_EXCL;
  mode_t mode = 0755;
  int fd = open(file_path.c_str(), flag, mode);
  if (fd == -1) {
    if (errno == EEXIST) {
      unlink(file_path.c_str());
    } else {
      LOG_ERR_MSG();
    }
    std::exit(errno);
  }
  char buffer[] = "sample data";
  int ret = write(fd, buffer, sizeof(buffer));
  if (ret == -1) {
    LOG_ERR_MSG();
    std::exit(errno);
  }
  ASSERT_EQ(ret - sizeof(buffer), 0U);
  memset(buffer, 0, sizeof(buffer));
  lseek(fd, 0, SEEK_SET);
  ret = read(fd, buffer, sizeof(buffer));
  if (ret == -1) {
    LOG_ERR_MSG();
    std::exit(errno);
  }
  ASSERT_STREQ(buffer, "sample data");

  int fd2 = fcntl(fd, F_DUPFD);
  ASSERT_NE(fd, fd2);
  memset(buffer, 0, sizeof(buffer));
  lseek(fd2, 0, SEEK_SET);
  ret = read(fd2, buffer, sizeof(buffer));
  ASSERT_STREQ(buffer, "sample data");

  close(fd);
  unlink(file_path.c_str());
}