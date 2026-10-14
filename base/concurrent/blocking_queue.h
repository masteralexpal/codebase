/**
 * @file blocking_queue.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONCURRENT_BLOCKING_QUEUE_H_
#define BASE_CONCURRENT_BLOCKING_QUEUE_H_

#include <queue>

#include "base/concurrent/condition_variable.h"
#include "base/concurrent/mutex.h"
#include "base/concurrent/mutex_guard.h"

namespace base {

template <typename T>
class BlockingQueue {
 public:
  BlockingQueue() = default;
  DISABLE_COPY(BlockingQueue);
  DISABLE_MOVE(BlockingQueue);
  virtual ~BlockingQueue() = default;

  void Enqueue(T&& t) {
    {
      MutexGuard<Mutex> guard(mutex_);
      queue_.emplace(t);
    }
    cv_.Broadcast();
  }

  void Enqueue(T const& t) {
    {
      MutexGuard<Mutex> guard(mutex_);
      queue_.push(t);
    }
    cv_.Broadcast();
  }

  T const& Front() const {
    MutexGuard<Mutex> guard(mutex_);
    return queue_.front();
  }

  T& Front() {
    MutexGuard<Mutex> guard(mutex_);
    return queue_.front();
  }

  T Dequeue() {
    mutex_.Acquire();
    while (queue_.empty()) {
      cv_.Wait(mutex_);
    }
    T t(std::move(queue_.front()));
    queue_.pop();
    mutex_.Release();
    return t;
  }

  std::size_t Size() const {
    MutexGuard<Mutex> guard(mutex_);
    return queue_.size();
  }

 private:
  Mutex mutable mutex_;
  ConditionVariable mutable cv_;
  std::queue<T> queue_;
};
}  // namespace base
#endif  //  BASE_CONCURRENT_BLOCKING_QUEUE_H_