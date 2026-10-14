/**
 * @file timestamp.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_TIMESTAMP_H_
#define BASE_TIMESTAMP_H_

#include <cstdint>
#include <string>

namespace base {
class Timestamp {
 public:
  static std::int64_t const kMicroSecondsPerSecond = 1000'000;
  Timestamp() noexcept;

  explicit Timestamp(std::int64_t ticks_since_epoch) noexcept;

  std::string ToString() const;

  std::string ToFormattedString() const;

  void Swap(Timestamp& other);

  bool IsValid() const noexcept;

  std::int64_t MicroSecondsSinceEpoch() const;
  time_t SecondsSinceEpoch() const;

  // Creation functions.
  static Timestamp Now();
  static Timestamp FromUnixTime(time_t unix_time);
  static Timestamp FromUnixTime(time_t unix_time, int microseconds);

  bool operator<(Timestamp other);

  bool operator==(Timestamp other);

  bool operator>(Timestamp other);

 private:
  std::int64_t ticks_since_epoch_ = 0;
};
}  // namespace base
#endif