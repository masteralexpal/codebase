/**
 * @file log_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/log.h"

#include <gtest/gtest.h>

namespace base {
class LogTestHelper : public testing::Test {
public:
  void SetUp() override {}

  void TearDown() override {}
};

TEST(LogMessage, message) {
  log::LogMessage(__FILE__, __LINE__, base::log::LOG_ERROR);
}

TEST(LogMessage, Macro) {
  LOG(DEBUG) << "DEBUG";
  LOG(INFO) << "INFO";
  LOG(WARNING) << "WARNING";
  LOG(ERROR) << "ERROR";
  LOG(FATAL) << "FATAL";
}

} // namespace base