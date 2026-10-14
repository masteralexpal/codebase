/**
 * @file task.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_CONCURRENT_TASK_H_
#define BASE_CONCURRENT_TASK_H_

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace base {

using Task = std::function<void()>;

template <typename Callable,
          typename... Args,
          typename std::enable_if<
              std::is_void<
                  typename std::invoke_result<Callable, Args...>::type>::value,
              bool>::type = true>
Task MakeTask(Callable&& callable, Args&&... args) {
  return Task([callable_ = std::move(callable),
               args_ = std::make_tuple(std::forward<Args>(args)...)]() -> void {
    std::apply(callable_, args_);
  });
}

// TODO
// class BindState {};
// class TaskBase {};
// class Task {};
}  // namespace base

#endif  // BASE_CONCURRENT_TASK_H_