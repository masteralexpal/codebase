/**
 * @file bio.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-03
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include <gtest/gtest.h>

#define PRINT_STANDARD_ERROR_CODE_AND_MESSAGE()                          \
  std::cerr << "[" << __FILE__ << "(" << __LINE__ << ")@" << __func__    \
            << "()] "                                                    \
            << "Error " << errno << " Occured: " << std::strerror(errno) \
            << std::endl;

bool FileExists(std::string const& file_path) {
  if (file_path.empty())
    return false;
  struct stat sb;
  return stat(file_path.c_str(), &sb) == 0 && !(sb.st_mode & S_IFDIR);
}

bool DirectoryExists(std::string const& directory_path) {
  if (directory_path.empty())
    return false;
  struct stat sb;
  return stat(directory_path.c_str(), &sb) == 0;
}

TEST(bio, open) {
  std::string test_file_path = "test_file";
  int flag = O_RDWR | O_CREAT | O_EXCL | O_CLOEXEC | O_TRUNC;
  mode_t mode = 00755;
  int fd = open(test_file_path.c_str(), flag, mode);
  if (fd == -1) {
    PRINT_STANDARD_ERROR_CODE_AND_MESSAGE();
  }
  struct stat sb;
  int result = stat(test_file_path.c_str(), &sb);
  ASSERT_EQ(result, 0);
  ASSERT_NE(fd, -1);
  ASSERT_TRUE(FileExists(test_file_path));
  ASSERT_EQ((sb.st_mode & 0777), mode);
  // Remove
  unlink(test_file_path.c_str());
}
