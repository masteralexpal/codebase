/**
 * @file closure_exp.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <cstddef>
#include <type_traits>

#include <gtest/gtest.h>

template <typename Tp, typename Sfinae = void>
struct IsCallable : std::false_type {};
template <typename Tp>
struct IsCallable<Tp, std::void_t<decltype(&Tp::operator())>> : std::true_type {
};

template <typename Functor, typename Sfinae>
struct FunctorTraits;
template <typename Functor, typename Sfinae = void>
struct FunctorTraits;
template <typename Functor>
struct FunctorTraits<
    Functor,
    typename std::enable_if<IsCallable<Functor>::value>::type> {};

template <typename Functor, typename... BoundArgs>
struct BindTraits {
  static constexpr size_t number_of_bound_arguments = sizeof...(BoundArgs);
  using functor_traits = FunctorTraits<Functor>;
  using invoke_type = typename functor_traits::invoke_type;
  // std::result_of was removed in C++20; std::invoke_result is the
  // replacement (note its arguments are <F, Args...>, not a signature type).
  using result_type = typename std::invoke_result<invoke_type>::type;
  using invoke_params_list = typename std::invoke_result<invoke_type>;
};

class A {
 public:
  int operator()(int, int);
};

class B {};

template <typename... Types>
struct TypeList;

template <size_t n, typename List>
struct DropTypeListItemImpl;

template <size_t n, typename T, typename... List>
struct DropTypeListItemImpl<n, TypeList<T, List...>>
    : DropTypeListItemImpl<n - 1, TypeList<List...>> {};

template <typename Signature>
struct __functor_traits {};
template <typename R, typename... Args>
struct __functor_traits<R(Args...)> {
  using return_type = R;
  using arguments_typelist = TypeList<Args...>;
};

TEST(A, B) {}