/**
 * @file task_queue.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-02-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_TASK_QUEUE_H_
#define BASE_TASK_QUEUE_H_

#include "base/concurrent/task.h"

namespace base {
class TaskQueue {
 public:
  TaskQueue() = default;

  virtual ~TaskQueue() = default;

  virtual void Push(Task task) = 0;

  virtual Task Pop() = 0;

  virtual std::size_t CurrentTasks() noexcept = 0;

  virtual std::size_t GetTaskLimit() noexcept = 0;
};
}  // namespace base

#endif