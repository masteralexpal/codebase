/**
 * @file path.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_FILE_PATH_H_
#define BASE_FILE_PATH_H_

#include <string>

namespace base {
class Path final {
 public:
  Path() = default;
  explicit Path(char const* pathname);
  explicit Path(std::string pathname);

  // Copy and move.
  Path(Path const& other);
  Path& operator=(Path const& other);
  Path(Path&& other) noexcept;
  Path& operator=(Path&& other) noexcept;

  // final
  ~Path() = default;

  bool Empty() const noexcept;

  bool IsAbsolute() const noexcept;

  void Append(char const* sub_path);

  void Append(std::string const& sub_path);

  void Append(Path const& sub_path);

  std::string PopBack();

  std::string ToString() const;

  static constexpr inline char GetPlatformSeprator() {
#if defined(__linux) || defined(__linux__) || defined(__gnu__linux__) || \
    defined(__APPLE__) || defined(__MACH__)
    return '/';
#elif
    return '\\';
#else
    return '/';
#endif
  }

 private:
  std::string path_name_;
};

}  // namespace base

#endif