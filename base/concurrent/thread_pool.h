#ifndef BASE_CONCURRENT_THREAD_POOL_H_
#define BASE_CONCURRENT_THREAD_POOL_H_

#include <functional>
#include <future>
#include <queue>
#include <type_traits>

namespace base {
class ThreadPool {
 public:
  struct CreationParams {
    int number_of_threads = 0;
    int thread_stack_size = 0;
  };
  static void Create(CreationParams const& params);

  static void Destroy();

  static ThreadPool* Get();

  ThreadPool() = delete;
  // non-copyable
  ThreadPool(ThreadPool const&) = delete;
  ThreadPool& operator=(ThreadPool const&) = delete;
  // non-movable
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  template <typename F, typename... Args>
  std::future<typename std::invoke_result<F, Args...>::type> Submit(
      F&& f,
      Args&&... args) {
    auto packaged_task = std::make_shared<
        std::packaged_task<typename std::invoke_result<F, Args...>::type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto future = packaged_task->get_future();
    {
      std::lock_guard<std::mutex> lock_guard{this->thread_pool_mutex_};
      this->task_queue_.emplace(
          std::move([packaged_task]() -> void { (*packaged_task)(); }));
    }
    this->thread_pool_cv_.notify_all();
    return future;
  }

 private:
  explicit ThreadPool(CreationParams const& params);

  virtual ~ThreadPool();

  std::mutex thread_pool_mutex_;
  std::condition_variable thread_pool_cv_;
  std::unique_ptr<std::thread> worker_thread_;
  std::queue<std::function<void()>> task_queue_;
  std::atomic_bool is_stopped;
};
}  // namespace base
#endif