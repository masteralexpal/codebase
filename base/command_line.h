/**
 * @file command_line.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_COMMAND_LINE_H_
#define BASE_COMMAND_LINE_H_

#include <map>
#include <optional>
#include <string>

#include "base/base_exports.h"

namespace base {
/**
 * @brief commanline argument parser.
 *        support:
 *          (1) --key
 *          (2) --key=val
 *          (3) -k,
 *          (4) -k=val
 *
 */

class BASE_EXPORT CommandLine final {
 public:
  using Key = std::string;
  using Value = std::string;

  static void Initialize(int argc, char* argv[]);

  static CommandLine* Get() noexcept;

  bool HasKey(Key const& key) const;

  std::optional<Value> GetValue(Key const& key) const;

 private:
  CommandLine();

  CommandLine(CommandLine const&) = delete;
  CommandLine& operator=(CommandLine const&) = delete;

  ~CommandLine();

  // TODO split args_ to args_ an kwargs_;
  std::map<Key, std::optional<Value>> args_;
};
}  // namespace base

#endif