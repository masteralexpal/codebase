/**
 * @file task_executor_impl.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "base/concurrent/task_executor_impl.h"

#include <pthread.h>
#include <iostream>
#include <mutex>
#include <tuple>
#include <utility>

#include "base/concurrent/task_queue_impl.h"
#include "base/log.h"

namespace base {

TaskExecutorImpl::TaskExecutorImpl()
    : task_queue_{nullptr},
      worker_threads_{},
      is_running_{false},
      number_of_idle_threads_{0},
      number_of_total_threads_{0},
      configuration_{} {
  task_queue_ = std::make_unique<TaskQueueImpl>(2);
}

TaskExecutorImpl::~TaskExecutorImpl() {
  if (is_running_.load(std::memory_order_acquire)) {
    Stop();
  }
}

bool TaskExecutorImpl::Start(Configuration const& configuration) {
  is_running_.store(true, std::memory_order_release);

  configuration_ = std::move(configuration);

  for (auto i = 0; i < configuration_.max_threads(); ++i) {
    this->worker_threads_.emplace_back([this]() -> void {
      while (true) {
        if (!this->is_running_.load(std::memory_order_acquire)) {
          return;
        }

        Task task = this->task_queue_->Pop();
        this->number_of_idle_threads_.fetch_sub(1);
        task();
        this->number_of_idle_threads_.fetch_add(1);
      }
    });
  }

  return true;
}

bool TaskExecutorImpl::Submit(Task task, TaskTraits const& task_traits) {
  LOG(INFO) << __func__ << "() called!";
  task_queue_->Push(task);
  std::ignore = task_traits;
  return true;
}

int TaskExecutorImpl::GetThreads() const noexcept {
  return number_of_total_threads_.load(std::memory_order_acquire);
}

int TaskExecutorImpl::GetIdleThreads() const noexcept {
  return number_of_idle_threads_.load(std::memory_order_acquire);
}

bool TaskExecutorImpl::Stop(Task callback) {
  LOG(WARNING)
      << "ThreadGroup is stopping, Waiting all worker threads to exit.";

  Task stopping_task{[this]() -> void { this->is_running_.store(false); }};

  auto total = GetThreads();
  for (auto i = 0; i < total; ++i) {
    task_queue_->Push(stopping_task);
  }

  for (auto&& thread : worker_threads_) {
    LOG(INFO) << "join()!";
    thread.join();
  }

  LOG(WARNING) << "All threads have finished.";

  if (callback)
    callback();
  return true;
}

bool TaskExecutorImpl::IsRuning() const noexcept {
  return is_running_;
}

}  // namespace base