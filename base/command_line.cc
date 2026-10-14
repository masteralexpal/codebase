/**
 * @file command_line.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/command_line.h"
#include "base/string_aux.h"

#include <cassert>
#include <iostream>
#include <mutex>
#include <optional>
#include <utility>

namespace base {

static CommandLine* g_instance = nullptr;

CommandLine::CommandLine() = default;

CommandLine::~CommandLine() = default;

// static
void CommandLine::Initialize(int argc, char* argv[]) {
  // Only initialize once.
  static std::once_flag flag;
  std::call_once(flag, [argc, argv]() noexcept -> void {
    g_instance = new CommandLine;
    g_instance->args_.emplace(argv[0], std::nullopt);
    for (int i = 1; i < argc; ++i) {
      if (argv[i]) {
        std::string trimed = Trim(argv[i]);
        if (StartsWith(trimed, "--")) {
          std::size_t position = trimed.find("=");
          if (position == std::string::npos) {
            auto [iter, success] = g_instance->args_.emplace(
                std::string{trimed.begin() + 2, trimed.end()}, std::nullopt);
            if (!success) {
              // maybe the key is alread used.
            }
          } else {
            auto [iter, success] = g_instance->args_.emplace(
                std::string{trimed.begin() + 2, trimed.begin() + position},
                std::string{trimed.begin() + position + 1, trimed.end()});
            if (!success) {
              // maybe the key is alread used.
            }
          }
        } else if (StartsWith(trimed, "-")) {
          std::size_t position = trimed.find("=");
          if (position == std::string::npos) {
            auto [iter, success] = g_instance->args_.emplace(
                std::string{trimed.begin() + 1, trimed.end()}, std::nullopt);
            if (!success) {
              // maybe the key is alread used.
            }
          } else {
            auto [iter, success] = g_instance->args_.emplace(
                std::string{trimed.begin() + 1, trimed.begin() + position},
                std::string{trimed.begin() + position + 1, trimed.end()});
            if (!success) {
              // maybe the key is alread used.
            }
          }
        } else {
          // TODO: LOG it.
          // Unreconginized command argumetn: tirmed;
        }
      }
    }
  });
  // TEST Code: dump to std::out
  // TODO: Using LOG instead.
  for (auto&& [key, val] : g_instance->args_) {
    std::cerr << "[ " << key;
    if (val) {
      std::cerr << ", " << val.value();
    }
    std::cerr << " ]" << std::endl;
  }
}

// static
CommandLine* CommandLine::Get() noexcept {
  return g_instance;
}

bool CommandLine::HasKey(Key const& key) const {
  return args_.find(key) != args_.cend();
}

std::optional<CommandLine::Value> CommandLine::GetValue(Key const& key) const {
  auto const iter = args_.find(key);
  return iter != args_.cend() ? iter->second : std::nullopt;
}

}  // namespace base