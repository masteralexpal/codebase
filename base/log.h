/**
 * @file log.h
 * @author lipingan (lipingan@hisense.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_LOG_H_
#define BASE_LOG_H_

#include <cstddef>
#include <ostream>
#include <sstream>
#include <string>

// [pid, tid, data, time, level, filename, line] Message

namespace base {

namespace log {

using LogSeverity = int;

constexpr LogSeverity LOG_VERBOSE = -1;
constexpr LogSeverity LOG_DEBUG = 0;
constexpr LogSeverity LOG_INFO = 1;
constexpr LogSeverity LOG_WARNING = 2;
constexpr LogSeverity LOG_ERROR = 3;
constexpr LogSeverity LOG_FATAL = 4;

class LogConfiguration final {
 public:
  constexpr LogConfiguration() = default;

  ~LogConfiguration() = default;

  void SetUseNanoseconds(bool flag) noexcept;

  bool GetUseNanoseconds() const noexcept;

  void SetMinLogSeverity(LogSeverity severity) noexcept;

  LogSeverity GetMinLogSeverity() const noexcept;

 private:
  bool use_nanoseconds_{false};

  LogSeverity min_log_severity_{LOG_WARNING};
};

class LogMessage final {
 public:
  LogMessage(char const* file, int line, LogSeverity severity) noexcept;

  LogMessage(char const* file,
             size_t file_length,
             int line,
             LogSeverity) noexcept;

  ~LogMessage();

  std::ostream& Stream() noexcept;

  std::string ToString() noexcept;

 private:
  LogSeverity severity_;
  std::ostringstream stream_;
};

class LogMessageVoidify {
 public:
  LogMessageVoidify() = default;

  void operator&(std::ostream&) {}
};

void Initialize(LogConfiguration const& logging_configuration);

bool ShouldCreateLogMessage(LogSeverity severity);

#define COMPACT_LOG_EX_DEBUG(ClassName, ...)                   \
  ::base::log::ClassName(__FILE__, sizeof(__FILE__), __LINE__, \
                         ::base::log::LOG_DEBUG)

#define COMPACT_LOG_EX_INFO(ClassName, ...)                    \
  ::base::log::ClassName(__FILE__, sizeof(__FILE__), __LINE__, \
                         ::base::log::LOG_INFO)

#define COMPACT_LOG_EX_WARNING(ClassName, ...)                 \
  ::base::log::ClassName(__FILE__, sizeof(__FILE__), __LINE__, \
                         ::base::log::LOG_WARNING)

#define COMPACT_LOG_EX_ERROR(ClassName, ...)                   \
  ::base::log::ClassName(__FILE__, sizeof(__FILE__), __LINE__, \
                         ::base::log::LOG_ERROR)

#define COMPACT_LOG_EX_FATAL(ClassName, ...)                   \
  ::base::log::ClassName(__FILE__, sizeof(__FILE__), __LINE__, \
                         ::base::log::LOG_FATAL)

#define COMPACT_LOG_DEBUG COMPACT_LOG_EX_DEBUG(LogMessage)
#define COMPACT_LOG_INFO COMPACT_LOG_EX_INFO(LogMessage)
#define COMPACT_LOG_WARNING COMPACT_LOG_EX_WARNING(LogMessage)
#define COMPACT_LOG_ERROR COMPACT_LOG_EX_ERROR(LogMessage)
#define COMPACT_LOG_FATAL COMPACT_LOG_EX_FATAL(LogMessage)

#define LOG_IS_ON(severity) \
  (::base::log::ShouldCreateLogMessage(::base::log::LOG_##severity))

#define LOG_STREAM(severity) COMPACT_LOG_##severity.Stream()

#define LAZY_STREAM(stream, condition) \
  !(condition) ? (void)0 : ::base::log::LogMessageVoidify() & (stream)

#define LOG(severity) LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity))

}  // namespace log
}  // namespace base

#endif