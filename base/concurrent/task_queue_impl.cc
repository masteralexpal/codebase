/**
 * @file fixed_task_queue_impl.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-02-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "base/concurrent/task_queue_impl.h"

namespace base {
void TaskQueueImpl::Push(Task task) {
  {
    std::unique_lock<std::mutex> unique_lock{queue_mutex_};

    while (task_queue_.size() >= task_limit_) {
      queue_cv_.wait(unique_lock);
    }
    task_queue_.emplace(std::move(task));
  }
  queue_cv_.notify_all();
}

Task TaskQueueImpl::Pop() {
  Task task;
  {
    std::unique_lock<std::mutex> unique_lock{queue_mutex_};

    while (task_queue_.size() == 0) {
      queue_cv_.wait(unique_lock);
    }

    task = std::move(task_queue_.front());
    task_queue_.pop();
  }

  queue_cv_.notify_all();
  return task;
}

std::size_t TaskQueueImpl::CurrentTasks() noexcept {
  std::lock_guard<std::mutex> lock_guard{queue_mutex_};
  return task_queue_.size();
}

std::size_t TaskQueueImpl::GetTaskLimit() noexcept {
  return task_limit_;
}

}  // namespace base