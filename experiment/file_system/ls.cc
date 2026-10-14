/**
 * @file ls.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <dirent.h>
#include <cstring>
#include <iostream>

int main(int argc, char** argv) {
  DIR* dir;
  struct dirent* directory_entity;
  if (argc == 1) {
    dir = opendir(".");
  } else {
    dir = opendir(argv[1]);
  }
  if (dir == nullptr) {
    std::cerr << "Cannot open directory: " << std::strerror(errno) << std::endl;
    std::exit(errno);
  }
  while ((directory_entity = readdir(dir)) != nullptr) {
    std::cout << directory_entity->d_name << std::endl;
  }
  closedir(dir);
  return 0;
}