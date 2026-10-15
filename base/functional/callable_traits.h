
#ifndef BASE_FUNCTIONAL_CALLABLE_TRAITS_H_
#define BASE_FUNCTIONAL_CALLABLE_TRAITS_H_

#include "base/functional/common_types.h"
#include "base/functional/type_list.h"

namespace base {

template <typename...>
static inline constexpr bool always_false_v = false;

template <typename Callable>
struct CallableTraits {
  static_assert(always_false_v<Callable>,
                "The provided type is not a supported callable (e.g., "
                "overloaded functors or generic lambdas).");
};

template <typename R, typename... Ps>
struct CallableTraits<R (*)(Ps...)> {
  using Return = R;
  using ParameterList = TypeList<Ps...>;
  using CanonicalParameterList = TypeList<Ps...>;

  static constexpr CallableCategory callable_category = CallableCategory::kFn;
  static constexpr bool is_noexcept = false;
};

template <typename R, typename... Ps>
struct CallableTraits<R(Ps...)> : CallableTraits<R (*)(Ps...)> {};

template <typename R, typename... Ps>
struct CallableTraits<R (&)(Ps...)> : CallableTraits<R (*)(Ps...)> {};

template <typename R, typename... Ps>
struct CallableTraits<R (*)(Ps...) noexcept> : CallableTraits<R (*)(Ps...)> {
  static constexpr bool is_noexcept = true;
};

template <typename R, typename... Ps>
struct CallableTraits<R(Ps...) noexcept>
    : CallableTraits<R (*)(Ps...) noexcept> {};

template <typename R, typename... Ps>
struct CallableTraits<R (&)(Ps...) noexcept>
    : CallableTraits<R (*)(Ps...) noexcept> {};

template <bool is_const_v,
          bool is_volatile_v,
          bool is_lv_ref_v,
          bool is_rv_ref_v,
          bool is_noexcept_v,
          typename R,
          typename C,
          typename... Ps>
struct CallableTraitsForMemFn {
  using Return = R;
  using Class = C;
  using ParameterList = TypeList<Ps...>;
  using CanonicalParameterList = TypeList<C*, Ps...>;

  static constexpr CallableCategory callable_category =
      CallableCategory::kMemFn;
  static constexpr bool is_const = is_const_v;
  static constexpr bool is_volatile = is_volatile_v;
  static constexpr bool is_lv_ref = is_lv_ref_v;
  static constexpr bool is_rv_ref = is_rv_ref_v;
  static constexpr bool is_noexcept = is_noexcept_v;
};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...)>
    : CallableTraitsForMemFn<false, false, false, false, false, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const>
    : CallableTraitsForMemFn<true, false, false, false, false, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) volatile>
    : CallableTraitsForMemFn<false, true, false, false, false, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const volatile>
    : CallableTraitsForMemFn<true, true, false, false, false, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) &>
    : CallableTraitsForMemFn<false, false, true, false, false, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const&>
    : CallableTraitsForMemFn<true, false, true, false, false, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) volatile&>
    : CallableTraitsForMemFn<false, true, true, false, false, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const volatile&>
    : CallableTraitsForMemFn<true, true, true, false, false, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) &&>
    : CallableTraitsForMemFn<false, false, false, true, false, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const&&>
    : CallableTraitsForMemFn<true, false, false, true, false, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) volatile&&>
    : CallableTraitsForMemFn<false, true, false, true, false, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const volatile&&>
    : CallableTraitsForMemFn<true, true, false, true, false, R, C, Ps...> {};

// noexcept
template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) noexcept>
    : CallableTraitsForMemFn<false, false, false, false, true, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const noexcept>
    : CallableTraitsForMemFn<true, false, false, false, true, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) volatile noexcept>
    : CallableTraitsForMemFn<false, true, false, false, true, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const volatile noexcept>
    : CallableTraitsForMemFn<true, true, false, false, true, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) & noexcept>
    : CallableTraitsForMemFn<false, false, true, false, true, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const & noexcept>
    : CallableTraitsForMemFn<true, false, true, false, true, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) volatile & noexcept>
    : CallableTraitsForMemFn<false, true, true, false, true, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const volatile & noexcept>
    : CallableTraitsForMemFn<true, true, true, false, true, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) && noexcept>
    : CallableTraitsForMemFn<false, false, false, true, true, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const && noexcept>
    : CallableTraitsForMemFn<true, false, false, true, true, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) volatile && noexcept>
    : CallableTraitsForMemFn<false, true, false, true, true, R, C, Ps...> {};

template <typename R, typename C, typename... Ps>
struct CallableTraits<R (C::*)(Ps...) const volatile && noexcept>
    : CallableTraitsForMemFn<true, true, false, true, true, R, C, Ps...> {};

// Functor Traits: Resolves classes via their operator() address.
// Only non-overloaded and non-generic functors are supported to avoid
// ambiguity.
template <typename Functor>
  requires requires { &std::remove_reference_t<Functor>::operator(); }
struct CallableTraits<Functor> {
 private:
  using PMFTraits =
      CallableTraits<decltype(&std::remove_reference_t<Functor>::operator())>;

 public:
  using Return = typename PMFTraits::Return;
  using ParameterList = typename PMFTraits::ParameterList;
  using CanonicalParameterList = typename PMFTraits::ParameterList;

  static constexpr bool is_const = PMFTraits::is_const;
  static constexpr bool is_volatile = PMFTraits::is_volatile;
  static constexpr bool is_lv_ref = PMFTraits::is_lv_ref;
  static constexpr bool is_rv_ref = PMFTraits::is_rv_ref;
  static constexpr bool is_noexcept = PMFTraits::is_noexcept;
  static constexpr CallableCategory callable_category =
      CallableCategory::kFunctor;
};

}  // namespace base

#endif  // BASE_FUNCTIONAL_CALLABLE_TRAITS_H_