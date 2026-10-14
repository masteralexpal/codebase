#include "base/concurrent/thread_pool.h"

namespace base {
static ThreadPool* g_thread_pool = nullptr;
static std::once_flag g_once_flag;
// static
void ThreadPool::Create(CreationParams const& params) {
  std::call_once(g_once_flag, [&params]() -> void {
    // creation params adjustments.
    CreationParams mutable_params = params;
    if (mutable_params.number_of_threads == 0) {
      mutable_params.number_of_threads = std::thread::hardware_concurrency();
    }
    // creation
    g_thread_pool = new ThreadPool(mutable_params);
  });
}
// static
void ThreadPool::Destroy() {
  delete g_thread_pool;
  g_thread_pool = nullptr;
}

ThreadPool* ThreadPool::Get() {
  return g_thread_pool;
}

ThreadPool::ThreadPool(CreationParams const& params) {
  (void)params;
  worker_thread_ = std::make_unique<std::thread>([this]() -> void {
    std::function<void()> currnet_task;
    while (!is_stopped) {
      {
        std::unique_lock<std::mutex> unique_lock{this->thread_pool_mutex_};
        // 如果任务队列为空，则需要进入wait
        while (this->task_queue_.empty()) {
          this->thread_pool_cv_.wait(unique_lock);
        }
        // this->thread_pool_cv_.wait(unique_lock, [this]() -> bool {
        //   return !this->task_queue_.empty();
        // });
        currnet_task = std::move(this->task_queue_.front());
        task_queue_.pop();
      }
      currnet_task();
    }
  });
  this->worker_thread_->detach();
}

ThreadPool::~ThreadPool() {
  is_stopped = true;
  this->thread_pool_cv_.notify_all();
}
}  // namespace base