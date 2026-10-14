/**
 * @file log.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/log.h"

#include <cstring>
#include <ctime>
#include <iomanip>

#include "base/build_config.h"

#if IS_WIN32 || IS_WIN64
constexpr char const kSlash = '\\';
#else
constexpr char const kSlash = '/';
#endif

#if IS_LINUX
#include <unistd.h>
static inline pid_t GetPid() {
  static pid_t currnet_pid = getpid();
  return currnet_pid;
}
#endif

#if IS_LINUX
#include <sys/syscall.h>
#if defined(SYS_gettid)
#define gettid() ((pid_t)syscall(SYS_gettid))
static inline pid_t GetTid() {
  thread_local pid_t currnet_tid = gettid();
  return currnet_tid;
}
#else
#error "SYS_gettid unavailable on this system"
#endif

#endif

namespace base {

namespace log {

static LogConfiguration g_log_configuration;

void LogConfiguration::SetUseNanoseconds(bool flag) noexcept {
  use_nanoseconds_ = flag;
}
bool LogConfiguration::GetUseNanoseconds() const noexcept {
  return use_nanoseconds_;
}

void LogConfiguration::SetMinLogSeverity(LogSeverity severity) noexcept {
  min_log_severity_ = severity;
}

LogSeverity LogConfiguration::GetMinLogSeverity() const noexcept {
  return min_log_severity_;
}

LogMessage::LogMessage(char const* file,
                       int line,
                       LogSeverity severity) noexcept
    : LogMessage(file, strlen(file), line, severity) {}

LogMessage::LogMessage(char const* file,
                       size_t file_length,
                       int line,
                       LogSeverity severity) noexcept
    : severity_(severity) {
  char const* reduced_file = file;
  for (auto i = file_length; i != 0; --i) {
    if (file[i] == kSlash) {
      reduced_file = &(file[i + 1]);
      break;
    }
  }
  stream_ << '[' << GetPid() << ':' << GetTid() << ' ';
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  tm* ptm = localtime(&ts.tv_sec);
  if (ptm) {
    stream_ << std::setw(4) << std::setfill('0') << ptm->tm_year + 1900 << '-'
            << std::setw(2) << std::setfill('0') << ptm->tm_mon + 1 << '-'
            << std::setw(2) << std::setfill('0') << ptm->tm_mday << 'T'
            << std::setw(2) << std::setfill('0') << ptm->tm_hour << ':'
            << std::setw(2) << std::setfill('0') << ptm->tm_min << ':'
            << std::setw(2) << std::setfill('0') << ptm->tm_sec << '.'
            << std::setw(9) << std::setfill('0') << ts.tv_nsec << ' ';
    switch (severity_) {
      case LOG_VERBOSE:
        stream_ << "VERBOSE";
        break;
      case LOG_DEBUG:
        stream_ << "DEBUG";
        break;
      case LOG_INFO:
        stream_ << "INFO";
        break;
      case LOG_WARNING:
        stream_ << "WARNING";
        break;
      case LOG_ERROR:
        stream_ << "ERROR";
        break;
      case LOG_FATAL:
        stream_ << "FATAL";
        break;
    };
    stream_ << ' ' << reduced_file << '(' << line << ')' << ']';
  }
}

LogMessage::~LogMessage() {
  stream_ << '\n';
  std::string message = stream_.str();
  write(STDERR_FILENO, message.c_str(), message.size());
}

std::ostream& LogMessage::Stream() noexcept {
  return stream_;
}

std::string LogMessage::ToString() noexcept {
  return stream_.str();
}

void Initialize(LogConfiguration const& logging_configuration) {
  g_log_configuration = logging_configuration;
}

bool ShouldCreateLogMessage(LogSeverity severity) {
  return severity >= g_log_configuration.GetMinLogSeverity();
}

}  // namespace log
}  // namespace base