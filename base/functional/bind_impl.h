#ifndef BASE_FUNCTIONAL_BIND_IMPL_H_
#define BASE_FUNCTIONAL_BIND_IMPL_H_

#include <cstddef>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include "base/functional/callable_traits.h"
#include "base/functional/closure.h"
#include "base/functional/common_types.h"
#include "base/functional/type_list.h"

// Implementation of BindOnce/BindRepeating (bind.h), ported from lattice
// with the lattice-specific wrapper types (UnretainedPtr/RefPtr/WeakPtr)
// stripped: only std wrappers (shared_ptr/weak_ptr/reference_wrapper) are
// recognized as member-function receivers. A tagging wrapper like
// Chromium's Unretained can be added here when a memory utilities header
// lands.

namespace base {
namespace impl {

// ─── Template Specialization Detector ──────────────────────────────────
template <typename T, template <typename...> class Template>
struct is_specialization_of : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_specialization_of<Template<Args...>, Template> : std::true_type {};

template <typename T, template <typename...> class Template>
inline constexpr bool is_specialization_of_v =
    is_specialization_of<T, Template>::value;

// ─── Signature Assembly ────────────────────────────────────────────────
// Combines a TypeList of parameters and a return type into the function
// signature R(Params...). TypeList itself stays decoupled from the
// signature concept.

template <typename List, typename Return>
struct MakeSignatureImpl;

template <typename Return, typename... Params>
struct MakeSignatureImpl<TypeList<Params...>, Return> {
  using type = Return(Params...);
};

template <typename List, typename Return>
using MakeSignature = typename MakeSignatureImpl<List, Return>::type;

// ─── Unwrap ────────────────────────────────────────────────────────────
// Unwraps recognized wrapper types back to what std::invoke wants at run
// time; everything else passes through untouched.

template <typename T>
decltype(auto) Unwrap(T&& t) {
  return std::forward<T>(t);
}

template <typename T>
T* Unwrap(std::shared_ptr<T> const& ptr) {
  return ptr.get();
}

// std::invoke would unwrap reference_wrapper by itself; doing it here
// keeps the tuple-unpacking step uniform.
template <typename T>
T& Unwrap(std::reference_wrapper<T> const& ref) {
  return ref.get();
}

template <typename Callable,
          typename BoundTuple,
          typename... Unbound,
          std::size_t... I>
decltype(auto) Invoke(Callable&& callable,
                      BoundTuple&& bound_tuple,
                      std::index_sequence<I...>,
                      Unbound&&... unbound) {
  return std::invoke(
      std::forward<Callable>(callable),
      Unwrap(std::get<I>(std::forward<BoundTuple>(bound_tuple)))...,
      std::forward<Unbound>(unbound)...);
}

// ─── std::weak_ptr Receiver (PMF only) ─────────────────────────────────
// A member function bound to a std::weak_ptr is a "weak callback": lock()
// before invoking, skip silently when the object is gone. Compile-time
// enforced to return void (see BindValidator), so the early return below
// never needs to fabricate a value.

template <typename T>
inline constexpr bool IsStdWeakPtrV =
    is_specialization_of_v<std::decay_t<T>, std::weak_ptr>;

template <typename Callable, typename BoundTuple>
inline constexpr bool HasStdWeakPtrReceiver = [] {
  using Traits = CallableTraits<std::decay_t<Callable>>;
  constexpr bool is_memfn =
      Traits::callable_category == CallableCategory::kMemFn;
  constexpr bool has_receiver =
      std::tuple_size_v<std::decay_t<BoundTuple>> > 0;

  if constexpr (is_memfn && has_receiver) {
    using Receiver = std::tuple_element_t<0, std::decay_t<BoundTuple>>;
    return IsStdWeakPtrV<Receiver>;
  }
  return false;
}();

// Locks the weak receiver; a null shared_ptr means the object is gone.
template <typename Callable, typename BoundTuple>
auto LockStdWeakPtrReceiver(Callable const&, BoundTuple const& t) {
  if constexpr (HasStdWeakPtrReceiver<Callable, BoundTuple>) {
    return std::get<0>(t).lock();
  } else {
    return std::shared_ptr<void>{};
  }
}

template <typename Callable,
          typename LockedPtr,
          typename BoundTuple,
          std::size_t... J,
          typename... Unbound>
decltype(auto) InvokeWithStdWeakPtrImpl(Callable&& callable,
                                        LockedPtr&& locked,
                                        BoundTuple&& bound_tuple,
                                        std::index_sequence<J...>,
                                        Unbound&&... unbound) {
  return std::invoke(
      std::forward<Callable>(callable), std::forward<LockedPtr>(locked),
      Unwrap(std::get<1 + J>(std::forward<BoundTuple>(bound_tuple)))...,
      std::forward<Unbound>(unbound)...);
}

template <typename Callable,
          typename BoundTuple,
          std::size_t... I,
          typename... Unbound>
decltype(auto) InvokeWithStdWeakPtr(Callable&& callable,
                                    BoundTuple&& bound_tuple,
                                    std::index_sequence<I...>,
                                    Unbound&&... unbound) {
  auto locked = LockStdWeakPtrReceiver(callable, bound_tuple);
  if (!locked) {
    return;  // R is guaranteed to be void by BindValidator.
  }

  // I... spans all bound args (0..N-1); the locked receiver replaces index
  // 0, so the extra bound args are re-indexed as 1+J over 0..N-2.
  constexpr std::size_t kExtraArgs = (sizeof...(I) > 0) ? sizeof...(I) - 1 : 0;

  return InvokeWithStdWeakPtrImpl(std::forward<Callable>(callable),
                                  std::move(locked),
                                  std::forward<BoundTuple>(bound_tuple),
                                  std::make_index_sequence<kExtraArgs>{},
                                  std::forward<Unbound>(unbound)...);
}

// ─── InvokerGenerator ──────────────────────────────────────────────────
// Generates the invoker lambda with the explicit signature R(Params...).
// The Once variant consumes its captures (moves them into the call); the
// Repeating variant invokes through lvalues so the state survives.

template <typename Signature>
struct InvokerGenerator;

template <typename R, typename... Params>
struct InvokerGenerator<R(Params...)> {
  template <typename Callable, typename BoundTuple, std::size_t... Indices>
  static auto CreateOnce(Callable&& c,
                         BoundTuple&& t,
                         std::index_sequence<Indices...>) {
    return [c = std::forward<Callable>(c),
            t = std::forward<BoundTuple>(t)](Params... params) mutable -> R {
      if constexpr (HasStdWeakPtrReceiver<Callable, BoundTuple>) {
        InvokeWithStdWeakPtr(std::move(c), std::move(t),
                             std::index_sequence<Indices...>{},
                             std::forward<Params>(params)...);
      } else if constexpr (std::is_void_v<R>) {
        Invoke(std::move(c), std::move(t), std::index_sequence<Indices...>{},
               std::forward<Params>(params)...);
      } else {
        return Invoke(std::move(c), std::move(t),
                      std::index_sequence<Indices...>{},
                      std::forward<Params>(params)...);
      }
    };
  }

  template <typename Callable, typename BoundTuple, std::size_t... Indices>
  static auto CreateRepeating(Callable&& c,
                              BoundTuple&& t,
                              std::index_sequence<Indices...>) {
    return [c = std::forward<Callable>(c),
            t = std::forward<BoundTuple>(t)](Params... params) mutable -> R {
      if constexpr (HasStdWeakPtrReceiver<Callable, BoundTuple>) {
        InvokeWithStdWeakPtr(c, t, std::index_sequence<Indices...>{},
                             std::forward<Params>(params)...);
      } else if constexpr (std::is_void_v<R>) {
        Invoke(c, t, std::index_sequence<Indices...>{},
               std::forward<Params>(params)...);
      } else {
        return Invoke(c, t, std::index_sequence<Indices...>{},
                      std::forward<Params>(params)...);
      }
    };
  }
};

// ─── BindValidator ─────────────────────────────────────────────────────
// Compile-time binding rules, surfaced as individual members so BindHelper
// can emit one precise static_assert per rule.

template <typename T>
struct IsMoveOnly : std::bool_constant<!std::is_copy_constructible_v<T> &&
                                       std::is_move_constructible_v<T>> {};

template <ClosureCategory closure_category,
          typename Traits,
          typename Callable,
          typename... BoundArgs>
struct BindValidator {
  using BoundList = TypeList<BoundArgs...>;
  using CanonicalParams = typename Traits::CanonicalParameterList;

  // Bound args cannot exceed the callable's parameter count.
  static constexpr bool kArityOk = BoundList::size <= CanonicalParams::size;

  // A repeating closure stores its bound args for repeated use, so they
  // must be copyable.
  static constexpr bool kNoMoveOnlyInRepeating =
      (closure_category == ClosureCategory::kRepeating)
          ? NoneOf<IsMoveOnly, BoundList>::value
          : true;

  // Member-function receivers must arrive through a recognized wrapper
  // (std::ref / std::shared_ptr / std::weak_ptr): a raw pointer says
  // nothing about lifetime, and a by-value receiver silently slices a
  // copy into the closure — both are rejected.
  static constexpr bool kReceiverCheck = []() constexpr {
    if constexpr (Traits::callable_category == CallableCategory::kMemFn &&
                  sizeof...(BoundArgs) > 0) {
      using BoundTupleType = std::tuple<std::decay_t<BoundArgs>...>;
      using DecayedReceiver = std::tuple_element_t<0, BoundTupleType>;

      constexpr bool is_wrapper =
          is_specialization_of_v<DecayedReceiver, std::shared_ptr> ||
          is_specialization_of_v<DecayedReceiver, std::weak_ptr> ||
          is_specialization_of_v<DecayedReceiver, std::reference_wrapper>;
      return is_wrapper;
    }
    return true;
  }();

  // A weak callback may be silently skipped (lock() failed), so a non-void
  // return would have to be fabricated — misleading. Void only.
  static constexpr bool kStdWeakPtrVoidReturn = []() constexpr {
    if constexpr (Traits::callable_category == CallableCategory::kMemFn &&
                  sizeof...(BoundArgs) > 0) {
      using BoundTupleType = std::tuple<std::decay_t<BoundArgs>...>;
      using Receiver = std::tuple_element_t<0, BoundTupleType>;
      if constexpr (IsStdWeakPtrV<Receiver>) {
        return std::is_void_v<typename Traits::Return>;
      }
    }
    return true;
  }();

  static constexpr bool value = kArityOk && kNoMoveOnlyInRepeating &&
                                kReceiverCheck && kStdWeakPtrVoidReturn;
};

// ─── BindHelper ─────────────────────────────────────────────────────────

template <ClosureCategory closure_category>
struct BindHelper {
  template <typename Callable, typename... BoundArgs>
  static auto Bind(Callable&& callable, BoundArgs&&... args) {
    using Traits = CallableTraits<std::decay_t<Callable>>;
    using Return = typename Traits::Return;
    using ParamsList = typename Traits::CanonicalParameterList;

    using Validator = BindValidator<closure_category, Traits,
                                    std::decay_t<Callable>, BoundArgs...>;

    static_assert(Validator::kArityOk,
                  "Too many bound arguments provided.");
    static_assert(Validator::kNoMoveOnlyInRepeating,
                  "RepeatingClosure cannot bind move-only types (e.g., "
                  "std::unique_ptr). Use BindOnce instead.");
    static_assert(Validator::kReceiverCheck,
                  "A member-function receiver must be bound through "
                  "std::ref(obj), std::shared_ptr, or std::weak_ptr; raw "
                  "pointers and by-value receivers are rejected because "
                  "their lifetime is unchecked.");
    static_assert(Validator::kStdWeakPtrVoidReturn,
                  "A std::weak_ptr receiver requires a void return type: "
                  "the callback is silently skipped when the object is "
                  "gone, so a returned value would have to be fabricated.");

    // Unbound parameters = canonical parameters minus the bound prefix.
    constexpr std::size_t kBoundArity = sizeof...(BoundArgs);
    static_assert(Validator::kArityOk);

    using UnboundList = Drop<ParamsList, kBoundArity>;
    using Signature = MakeSignature<UnboundList, Return>;

    using TupleType = std::tuple<std::decay_t<BoundArgs>...>;
    TupleType bound_tuple{std::forward<BoundArgs>(args)...};

    using Generator = InvokerGenerator<Signature>;

    if constexpr (closure_category == ClosureCategory::kOnce) {
      auto invoker = Generator::CreateOnce(
          std::forward<Callable>(callable), std::move(bound_tuple),
          std::make_index_sequence<kBoundArity>{});
      return OnceClosure<Signature>(std::move(invoker));
    } else {
      auto invoker = Generator::CreateRepeating(
          std::forward<Callable>(callable), std::move(bound_tuple),
          std::make_index_sequence<kBoundArity>{});
      return RepeatingClosure<Signature>(std::move(invoker));
    }
  }
};

}  // namespace impl
}  // namespace base

#endif  // BASE_FUNCTIONAL_BIND_IMPL_H_
