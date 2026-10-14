#ifndef BASE_CLOSURE_H_
#define BASE_CLOSURE_H_

#include <tuple>
#include <type_traits>
#include <utility>
#include "base/macro.h"
namespace base {
// A naive implementaion.
template <typename Callable, typename... Args>
class Closure {
 public:
  // Get the type of return value of Callbale.
  using ResultType = typename std::invoke_result<Callable, Args...>::type;

  Closure(Callable&& callbale, Args&&... args)
      : callbale_(std::forward<Callable>(callbale)),
        args_(std::forward<Args>(args)...) {}

  DISABLE_COPY(Closure);

  Closure(Closure&& other) noexcept
      : callbale_(std::move(other.callbale_)), args_(std::move(args_)) {}

  Closure& operator=(Closure&& other) noexcept {
    if (&other != this) {
      callbale_ = std::move(other.callbale_);
      args_ = std::move(other.args_);
    }
    return *this;
  }

  ResultType operator()() && { return std::apply(callbale_, args_); }

 private:
  // TODO: Consider using std::decay_t<Callable>?
  Callable callbale_;
  std::tuple<Args...> args_;
};

// Helper function to produce base::Closure.
template <typename Callable, typename... Args>
auto MakeClosure(Callable&& callable, Args... args) {
  return Closure<Callable, Args...>(std::forward<Callable>(callable),
                                    std::forward<Args>(args)...);
}

// TODO
template <typename Callbale, typename... Args>
class PersisentClosure {};

}  // namespace base
#endif