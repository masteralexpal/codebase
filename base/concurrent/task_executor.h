/**
 * @file task_executor.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_CONCURRENT_TASK_EXECUTOR_H_
#define BASE_CONCURRENT_TASK_EXECUTOR_H_

#include "base/concurrent/task.h"
#include "base/macro.h"

namespace base {

class TaskExecutor {
 public:
  enum class TaskPriority { LOWEST, NROMAL, HIGHEST };

  class TaskTraits {
   public:
    inline TaskTraits() : task_priority_(TaskPriority::NROMAL) {}

    explicit inline TaskTraits(TaskPriority task_priority)
        : task_priority_(task_priority) {}

    TaskPriority task_priority() const noexcept { return task_priority_; }

   private:
    TaskPriority task_priority_;
  };

  TaskExecutor() = default;

  DISABLE_COPY(TaskExecutor);
  DISABLE_MOVE(TaskExecutor);

  virtual ~TaskExecutor() = default;

  virtual bool Submit(Task task, TaskTraits const& task_traits = {}) = 0;
};

}  // namespace base
#endif