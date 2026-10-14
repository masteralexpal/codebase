#include "base/concurrent/condition_variable.h"

#include "base/concurrent/mutex.h"
#include "base/concurrent/mutex_guard.h"
#include "base/concurrent/thread.h"
#include <gtest/gtest.h>

#include <chrono>
#include <queue>
#include <thread>

namespace base {
TEST(ConditionVariable, Queue) {
  std::queue<int> queue_;
  base::Mutex mutex;
  base::ConditionVariable cv;
  auto consumer = [&]() mutable -> void {
    while (true) {
      // Acquire
      mutex.Acquire();
      while (queue_.empty()) {
        // Release and then Acquire.
        cv.Wait(mutex);
      }
      while (!queue_.empty()) {
        std::cout << "CONSUMING: queue size = " << queue_.size() << std::endl;
        queue_.pop();
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
      }
      // Release
      mutex.Release();
    }
  };

  auto producer = [&]() mutable -> void {
    while (true) {
      {
        MutexGuard<Mutex> guard(mutex);
        while (queue_.size() < 10) {
          std::cout << "PRODUCING: queue size = " << queue_.size() << std::endl;
          queue_.push(1);
          std::this_thread::sleep_for(std::chrono::milliseconds{100});
        }
      }
      cv.Signal();
    }
  };

  base::Thread producer_thread{"producer"};
  base::Thread consumer_thread{"consumer"};

  producer_thread.Start(producer);
  consumer_thread.Start(consumer);

  producer_thread.Join();
  consumer_thread.Join();
}
} // namespace base