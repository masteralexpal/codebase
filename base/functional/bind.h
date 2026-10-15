#pragma once

#ifndef BASE_FUNCTIONAL_BIND_H_
#define BASE_FUNCTIONAL_BIND_H_

#include "base/functional/bind_impl.h"
#include "base/functional/common_types.h"

namespace base {

// std::bind's modern shape: binds a callable and a prefix of its arguments
// into a closure. BindOnce produces the move-only currency (move-only
// bound args welcome); BindRepeating requires copyable bound args and can
// run repeatedly.
template <typename Callable, typename... BoundArguments>
inline auto BindOnce(Callable&& callable, BoundArguments&&... bound_arguments) {
  return impl::BindHelper<ClosureCategory::kOnce>::Bind(
      std::forward<Callable>(callable),
      std::forward<BoundArguments>(bound_arguments)...);
}

template <typename Callable, typename... BoundArguments>
inline auto BindRepeating(Callable&& callable,
                          BoundArguments&&... bound_arguments) {
  return impl::BindHelper<ClosureCategory::kRepeating>::Bind(
      std::forward<Callable>(callable),
      std::forward<BoundArguments>(bound_arguments)...);
}

}  // namespace base

#endif  // BASE_FUNCTIONAL_BIND_H_
