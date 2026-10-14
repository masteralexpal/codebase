/**
 * @file task_queue_impl.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-02-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_CONCURRENT_TASK_QUEUE_IMPL_H_
#define BASE_CONCURRENT_TASK_QUEUE_IMPL_H_

#include <condition_variable>
#include <mutex>
#include <queue>

#include "base/concurrent/task_queue.h"

namespace base {
class TaskQueueImpl : public TaskQueue {
 public:
  inline TaskQueueImpl(std::size_t task_limit)
      : task_limit_(task_limit), queue_mutex_(), queue_cv_(), task_queue_() {}

  ~TaskQueueImpl() override = default;

  void Push(Task task) override;

  Task Pop() override;

  std::size_t CurrentTasks() noexcept override;

  std::size_t GetTaskLimit() noexcept override;

 private:
  std::size_t const task_limit_;
  std::mutex queue_mutex_;
  std::condition_variable queue_cv_;
  std::queue<Task> task_queue_;
};
}  // namespace base
#endif