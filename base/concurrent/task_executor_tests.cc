/**
 * @file task_executor_tests.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <chrono>
#include <iostream>
#include <tuple>

#include "base/concurrent/task.h"
#include "base/concurrent/task_executor_impl.h"
#include "base/concurrent/thread.h"

#include <gtest/gtest.h>
#include "base/log.h"

void Add(int a, int b) {
  LOG(INFO) << "LWPID = " << base::CurrentThread::GetKernelHandle()
            << ", result = " << a + b;
}

TEST(TaskExecutorImpl, Submit) {
  base::log::LogConfiguration lc;
  lc.SetMinLogSeverity(base::log::LOG_DEBUG);
  base::log::Initialize(lc);

  LOG(INFO) << "TaskExecutorImpl created";
  base::TaskExecutorImpl task_executor_impl;
  base::ThreadGroup::Configuration config{"Main", 10, 0};
  task_executor_impl.Start(config);

  for (auto i = 0; i < 100; ++i) {
    auto task = base::MakeTask(Add, i, 0);
    task_executor_impl.Submit(std::move(task));
  }

  task_executor_impl.Stop();

  base::CurrentThread::SleepFor(std::chrono::seconds{15});
}