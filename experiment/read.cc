/**
 * @file read.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cerrno>
#include <cstring>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>

#include "base/command_line.h"

constexpr std::size_t kBufferSize = 16;
char buffer[kBufferSize];

void PrintHelp() {
  std::cout << "read tools.";
}

int main(int argc, char** argv) {
  base::CommandLine::Initialize(argc, argv);
  auto path = base::CommandLine::Get()->GetValue("file");
  if (path) {
    int fd = open(path->c_str(), O_RDONLY);
    if (fd == -1) {
      std::cout << strerror(errno);
    }

    if (base::CommandLine::Get()->HasKey("b")) {
    } else {
      int result = 0;
      while ((result = read(fd, buffer, kBufferSize - 1))) {
        if (result == -1) {
          if (errno == EAGAIN) {
            continue;
          } else {
            std::cerr << strerror(errno);
            break;
          }
        }
        buffer[result] = '\0';
        std::cout << buffer;
      }
    }
    close(fd);
  } else {
    std::cout << "Arguments error!" << std::endl;
  }
}