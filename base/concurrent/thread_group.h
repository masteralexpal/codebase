#ifndef BASE_CONCURRENT_THREAD_GROUP_H_
#define BASE_CONCURRENT_THREAD_GROUP_H_

#include "base/concurrent/condition_variable.h"
#include "base/concurrent/mutex.h"
#include "base/concurrent/mutex_guard.h"
#include "base/concurrent/task.h"
#include "base/concurrent/thread.h"
#include "base/macro.h"

#include <functional>
#include <future>
#include <memory>
#include <queue>
#include <type_traits>
#include <vector>

namespace base {
class ThreadGroup {
 public:
  class Configuration {
   public:
    Configuration() = default;

    Configuration(std::string const& name, int max_threads, int max_tasks)
        : name_(name), max_threads_(max_threads), max_tasks_(max_tasks) {}

    std::string name() { return name_; }

    int max_threads() { return max_threads_; }

    int max_tasks() { return max_tasks_; }

   private:
    std::string name_ = "Anonymous";
    int max_threads_;
    int max_tasks_;
  };

  ThreadGroup() = default;

  DISABLE_COPY(ThreadGroup);
  DISABLE_MOVE(ThreadGroup);

  virtual ~ThreadGroup() = default;

  virtual bool Start(Configuration const& configuration) = 0;

  virtual bool Stop(Task callback) = 0;

  // virtual bool StopAtOnce(Task callback) = 0;

  virtual bool IsRuning() const noexcept = 0;

  virtual int GetThreads() const noexcept = 0;

  virtual int GetIdleThreads() const noexcept = 0;
};

}  // namespace base
#endif