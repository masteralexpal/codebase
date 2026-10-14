/**
 * @file simple_cipher.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <optional>
#include <vector>
#include "base/command_line.h"
#include "base/log.h"

namespace crypto {
void EncryptBuffer(std::uint8_t* buffer,
                   std::size_t size,
                   std::vector<std::uint8_t> const& passphrase) {
  std::size_t round = 1;
  std::size_t phrase_index = 0;
  std::uint8_t phrase_char = passphrase[0];
  std::uint8_t disturbance = 0;

  for (std::size_t i = 0; i < size; ++i) {
    disturbance = passphrase[round];
    std::uint8_t new_char = phrase_char ^ disturbance;
    buffer[i] ^= new_char;
    phrase_index += (round % 3) + 1;
    if (phrase_index >= passphrase.size()) {
      phrase_index = 0;
      ++round;
      if (round >= passphrase.size()) {
        round = 0;
      }
    }
    phrase_char = passphrase[phrase_index];
  }
}

}  // namespace crypto

int main(int argc, char** argv) {
  base::CommandLine::Initialize(argc, argv);
  base::CommandLine::Initialize(argc, argv);
  std::optional<std::string> in = base::CommandLine::Get()->GetValue("in");
  if (!in) {
    std::cerr << "No input file!" << std::endl;
    return -1;
  }

  FILE* fp = fopen(in->c_str(), "r");
  if (!fp) {
    std::cerr << "Input file open failed!" << std::strerror(errno) << "\n";
    return errno;
  }

  fseek(fp, 0, SEEK_END);
  auto size = static_cast<std::size_t>(ftell(fp));
  fseek(fp, 0, SEEK_SET);
  std::uint8_t* buffer = new std::uint8_t[size + 1];
  std::memset(buffer, 0, size + 1);
  if (!(fread(buffer, size, 1, fp) == size) == 0) {
    std::cerr << "Failed to read file correctly! " << std::strerror(errno)
              << "\n";
    return errno;
  }
  std::vector<std::uint8_t> passphrase = {72,  105, 115, 101, 110, 115,
                                          101, 95,  50,  48,  50,  51};
  crypto::EncryptBuffer(buffer, size, passphrase);
  std::optional<std::string> path = base::CommandLine::Get()->GetValue("out");
  if (!path) {
    std::cout << "No output path, will print to standard output." << '\n';
    std::cout << buffer;
  }
  FILE* out = fopen("enc_private_key.bin", "w+");
  fwrite(buffer, size, 1, out);
}