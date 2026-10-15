#ifndef BASE_FUNCTIONAL_CLOSURE_H_
#define BASE_FUNCTIONAL_CLOSURE_H_

#include <cassert>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#include "base/functional/move_only_function.h"

namespace base {

template <typename Signature>
class OnceClosure;

template <typename Signature>
class RepeatingClosure;

// A type-erased callable consumed by Run(): after std::move(closure).Run()
// the closure is empty. Modeled on Chromium's OnceClosure: the move-only
// currency of the task world — a task runs exactly once, so it never
// pretends to be copyable (std::function would force that lie and exclude
// move-only captures like unique_ptr).
template <typename R, typename... Params>
class OnceClosure<R(Params...)> final {
 public:
  using Return = R;
  using Signature = R(Params...);
  using FnOnce = MoveOnlyFunction<Signature>;

  constexpr OnceClosure() noexcept = default;
  explicit OnceClosure(FnOnce fn_once) : fn_once_(std::move(fn_once)) {}

  OnceClosure(std::nullptr_t) = delete;
  OnceClosure(OnceClosure const&) = delete;
  OnceClosure& operator=(OnceClosure const&) = delete;
  OnceClosure(OnceClosure&&) noexcept = default;
  OnceClosure& operator=(OnceClosure&&) noexcept = default;
  ~OnceClosure() = default;

  // Run() exists only on rvalues: consuming the closure must be spelled
  // std::move(closure).Run(), never silently.
  Return Run(Params...) & = delete;

  Return Run(Params... params) && {
    assert(fn_once_ && "Run() on an empty or already-consumed OnceClosure");
    return std::exchange(fn_once_, FnOnce{})(std::forward<Params>(params)...);
  }

  bool is_null() const noexcept { return !fn_once_; }
  void Reset() noexcept { fn_once_ = FnOnce{}; }
  explicit operator bool() const noexcept { return !!fn_once_; }

  // Chains a follow-up: the result of this closure feeds (or gates, when
  // void) f. Consumes this closure.
  template <typename F>
  auto Then(F&& f) && {
    using FDecayed = std::decay_t<F>;
    if constexpr (std::is_void_v<Return>) {
      using NextReturn = std::invoke_result_t<FDecayed>;
      return OnceClosure<NextReturn(Params...)>(
          [first = std::move(*this),
           next = std::forward<F>(f)](Params... params) mutable -> NextReturn {
            std::move(first).Run(std::forward<Params>(params)...);
            if constexpr (std::is_void_v<NextReturn>) {
              next();
            } else {
              return next();
            }
          });
    } else {
      using NextReturn = std::invoke_result_t<FDecayed, Return>;
      return OnceClosure<NextReturn(Params...)>(
          [first = std::move(*this),
           next = std::forward<F>(f)](Params... params) mutable -> NextReturn {
            return next(std::move(first).Run(std::forward<Params>(params)...));
          });
    }
  }

 private:
  FnOnce fn_once_;
};

// The copyable counterpart, for callables meant to run repeatedly. Reach
// for it only when a real consumer needs repetition (timers, subscriptions)
// — OnceClosure is the default currency.
//
// Then has two overloads: the lvalue one copies this closure and delegates
// to the rvalue one, where the actual chaining logic lives.
template <typename R, typename... Params>
class RepeatingClosure<R(Params...)> final {
 public:
  using Return = R;
  using Signature = R(Params...);
  using Fn = std::function<Signature>;

  constexpr RepeatingClosure() noexcept = default;
  explicit RepeatingClosure(Fn fn) : fn_(std::move(fn)) {}

  RepeatingClosure(RepeatingClosure const&) = default;
  RepeatingClosure& operator=(RepeatingClosure const&) = default;
  RepeatingClosure(RepeatingClosure&&) noexcept = default;
  RepeatingClosure& operator=(RepeatingClosure&&) noexcept = default;
  ~RepeatingClosure() = default;

  Return Run(Params... params) & {
    return fn_(std::forward<Params>(params)...);
  }

  // Running an rvalue still consumes it: an expiring closure should not
  // leave a live copy of itself behind by accident.
  Return Run(Params... params) && {
    return std::exchange(fn_, Fn{})(std::forward<Params>(params)...);
  }

  bool is_null() const noexcept { return !fn_; }
  void Reset() noexcept { fn_ = Fn{}; }
  explicit operator bool() const noexcept { return !!fn_; }

  template <typename F>
  auto Then(F&& f) & {
    return std::move(RepeatingClosure{*this}).Then(std::forward<F>(f));
  }

  template <typename F>
  auto Then(F&& f) && {
    using FDecayed = std::decay_t<F>;
    if constexpr (std::is_void_v<Return>) {
      using NextReturn = std::invoke_result_t<FDecayed>;
      return RepeatingClosure<NextReturn(Params...)>(
          [original = std::move(*this),
           next = std::forward<F>(f)](Params... params) mutable -> NextReturn {
            original.Run(std::forward<Params>(params)...);
            if constexpr (std::is_void_v<NextReturn>) {
              next();
            } else {
              return next();
            }
          });
    } else {
      using NextReturn = std::invoke_result_t<FDecayed, Return>;
      return RepeatingClosure<NextReturn(Params...)>(
          [original = std::move(*this),
           next = std::forward<F>(f)](Params... params) mutable -> NextReturn {
            return next(original.Run(std::forward<Params>(params)...));
          });
    }
  }

  // Conversion to OnceClosure, consuming this closure. A repeating closure
  // can always play the role of a once closure, not vice versa.
  operator OnceClosure<R(Params...)>() && {
    return OnceClosure<R(Params...)>(std::move(fn_));
  }

 private:
  Fn fn_;
};

}  // namespace base

#endif  // BASE_FUNCTIONAL_CLOSURE_H_
