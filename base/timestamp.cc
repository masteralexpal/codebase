/**
 * @file timestamp.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "base/timestamp.h"

#include <cstring>

#include <inttypes.h>
#include <sys/time.h>
#include <time.h>

namespace base {
Timestamp::Timestamp() noexcept = default;

Timestamp::Timestamp(std::int64_t ticks_since_epoch) noexcept
    : ticks_since_epoch_(ticks_since_epoch) {}

void Timestamp::Swap(Timestamp& other) {
  std::swap(ticks_since_epoch_, other.ticks_since_epoch_);
}

std::string Timestamp::ToString() const {
  static char buf[32];
  std::memset(&buf, 0, sizeof(buf));
  int64_t seconds = ticks_since_epoch_ / kMicroSecondsPerSecond;
  int64_t microseconds = ticks_since_epoch_ % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", seconds,
           microseconds);
  return buf;
}

std::string Timestamp::ToFormattedString() const {
  static char buf[64];
  std::memset(buf, 0, sizeof(buf));
  time_t seconds =
      static_cast<time_t>(ticks_since_epoch_ / kMicroSecondsPerSecond);
  tm tm_time;
  gmtime_r(&seconds, &tm_time);

  int microseconds =
      static_cast<int>(ticks_since_epoch_ % kMicroSecondsPerSecond);
  snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
           tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
           tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);

  return buf;
}

bool Timestamp::IsValid() const noexcept {
  return ticks_since_epoch_ > 0;
}

std::int64_t Timestamp::MicroSecondsSinceEpoch() const {
  return ticks_since_epoch_;
}

time_t Timestamp::SecondsSinceEpoch() const {
  return static_cast<time_t>(ticks_since_epoch_ / kMicroSecondsPerSecond);
}

Timestamp Timestamp::Now() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  int64_t seconds = tv.tv_sec;
  return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

Timestamp Timestamp::FromUnixTime(time_t unix_time) {
  return FromUnixTime(unix_time, 0);
}

Timestamp Timestamp::FromUnixTime(time_t unix_time, int microseconds) {
  return Timestamp(static_cast<int64_t>(unix_time) * kMicroSecondsPerSecond +
                   microseconds);
}

bool Timestamp::operator<(Timestamp other) {
  return ticks_since_epoch_ < other.ticks_since_epoch_;
}

bool Timestamp::operator==(Timestamp other) {
  return ticks_since_epoch_ == other.ticks_since_epoch_;
}

bool Timestamp::operator>(Timestamp other) {
  return ticks_since_epoch_ > other.ticks_since_epoch_;
}
}  // namespace base