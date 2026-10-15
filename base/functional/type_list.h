
#ifndef BASE_FUNCTIONAL_TYPE_LIST_H_
#define BASE_FUNCTIONAL_TYPE_LIST_H_

#include <cstddef>
#include <type_traits>

namespace base {
//==============================================================================
// TypeList
//==============================================================================

template <typename...>
struct TypeList;

template <typename... Types>
struct TypeList {
  static constexpr std::size_t size = sizeof...(Types);
};

//==============================================================================
// Concepts & Traits
//==============================================================================

template <typename T>
struct IsTypeList : std::false_type {};

template <typename... Types>
struct IsTypeList<TypeList<Types...>> : std::true_type {};

template <typename T>
concept TypeListLike = IsTypeList<T>::value;

//==============================================================================
// Structural Operations
//==============================================================================

// Prepend --------------------------------------------------------------------

template <TypeListLike List, typename New>
struct PrependImpl;

template <typename... Types, typename New>
struct PrependImpl<TypeList<Types...>, New> {
  using type = TypeList<New, Types...>;
};

template <TypeListLike List, typename New>
using Prepend = typename PrependImpl<List, New>::type;

// Concat ---------------------------------------------------------------------

template <TypeListLike List1, TypeListLike List2>
struct ConcatImpl;

template <typename... Types1, typename... Types2>
struct ConcatImpl<TypeList<Types1...>, TypeList<Types2...>> {
  using type = TypeList<Types1..., Types2...>;
};

template <TypeListLike List1, TypeListLike List2>
using Concat = typename ConcatImpl<List1, List2>::type;

// Drop / Take ----------------------------------------------------------------

// Drop -----------------------------------------------------------------------

template <TypeListLike List, std::size_t N>
struct DropImpl;

template <typename Head, typename... Tail, std::size_t N>
  requires(N > 0u)
struct DropImpl<TypeList<Head, Tail...>, N>
    : DropImpl<TypeList<Tail...>, N - 1u> {};

template <TypeListLike List>
struct DropImpl<List, 0u> {
  using type = List;
};

template <TypeListLike List, std::size_t N>
  requires(N <= List::size)
using Drop = typename DropImpl<List, N>::type;

// Take -----------------------------------------------------------------------

template <TypeListLike List, std::size_t N, TypeListLike TakenList = TypeList<>>
struct TakeImpl;

template <typename Head,
          typename... Tail,
          std::size_t N,
          typename... TakenTypes>
  requires(N > 0u)
struct TakeImpl<TypeList<Head, Tail...>, N, TypeList<TakenTypes...>>
    : TakeImpl<TypeList<Tail...>, N - 1u, TypeList<TakenTypes..., Head>> {};

template <TypeListLike List, TypeListLike TakenList>
struct TakeImpl<List, 0u, TakenList> {
  using type = TakenList;
};

template <TypeListLike List, std::size_t N>
  requires(N <= List::size)
using Take = typename TakeImpl<List, N>::type;

//==============================================================================
// Indexing & Replacement
//==============================================================================

// At -------------------------------------------------------------------------

template <TypeListLike List, std::size_t N>
struct AtImpl;

template <typename Head, typename... Tail, std::size_t N>
  requires(N > 0u)
struct AtImpl<TypeList<Head, Tail...>, N> : AtImpl<TypeList<Tail...>, N - 1u> {
};

template <typename Head, typename... Tail>
struct AtImpl<TypeList<Head, Tail...>, 0u> {
  using type = Head;
};

template <TypeListLike List, std::size_t N>
  requires(N < List::size)
using At = typename AtImpl<List, N>::type;

// Replace --------------------------------------------------------------------

template <TypeListLike List, std::size_t N, typename New>
struct ReplaceImpl;

template <typename Head, typename... Tail, std::size_t N, typename New>
  requires(N > 0u)
struct ReplaceImpl<TypeList<Head, Tail...>, N, New> {
  using type =
      Prepend<typename ReplaceImpl<TypeList<Tail...>, N - 1u, New>::type, Head>;
};

template <typename Head, typename... Tail, typename New>
struct ReplaceImpl<TypeList<Head, Tail...>, 0u, New> {
  using type = TypeList<New, Tail...>;
};

template <TypeListLike List, std::size_t N, typename New>
  requires(N < List::size)
using Replace = typename ReplaceImpl<List, N, New>::type;

//==============================================================================
// Transformations
//==============================================================================

// Map ------------------------------------------------------------------------

template <template <typename> typename F, TypeListLike List>
struct MapImpl;

template <template <typename> typename F, typename... Ts>
struct MapImpl<F, TypeList<Ts...>> {
  using type = TypeList<typename F<Ts>::type...>;
};

template <template <typename> typename F, TypeListLike List>
using Map = typename MapImpl<F, List>::type;

// Filter ---------------------------------------------------------------------

template <template <typename> typename Predicate, TypeListLike List>
struct FilterImpl;

template <template <typename> typename Predicate>
struct FilterImpl<Predicate, TypeList<>> {
  using type = TypeList<>;
};

template <template <typename> typename Predicate,
          typename Head,
          typename... Tail>
struct FilterImpl<Predicate, TypeList<Head, Tail...>> {
  using TailFiltered = typename FilterImpl<Predicate, TypeList<Tail...>>::type;
  using type = std::conditional_t<Predicate<Head>::value,
                                  Prepend<TailFiltered, Head>,
                                  TailFiltered>;
};

template <template <typename> typename Predicate, TypeListLike List>
using Filter = typename FilterImpl<Predicate, List>::type;

//==============================================================================
// Fold (type-level Reduce)
//==============================================================================

// FoldLeft -------------------------------------------------------------------

template <template <typename, typename> typename F,
          typename Acc,
          TypeListLike List>
struct FoldLeftImpl;

template <template <typename, typename> typename F,
          typename Acc,
          typename Head,
          typename... Tail>
struct FoldLeftImpl<F, Acc, TypeList<Head, Tail...>>
    : FoldLeftImpl<F, typename F<Acc, Head>::type, TypeList<Tail...>> {};

template <template <typename, typename> typename F, typename Acc>
struct FoldLeftImpl<F, Acc, TypeList<>> {
  using type = Acc;
};

template <template <typename, typename> typename F,
          typename Init,
          TypeListLike List>
using FoldLeft = typename FoldLeftImpl<F, Init, List>::type;

// FoldRight ------------------------------------------------------------------

template <template <typename, typename> typename F,
          TypeListLike List,
          typename Acc>
struct FoldRightImpl;

template <template <typename, typename> typename F,
          typename Head,
          typename... Tail,
          typename Acc>
struct FoldRightImpl<F, TypeList<Head, Tail...>, Acc> {
  using type =
      typename F<Head,
                 typename FoldRightImpl<F, TypeList<Tail...>, Acc>::type>::type;
};

template <template <typename, typename> typename F, typename Acc>
struct FoldRightImpl<F, TypeList<>, Acc> {
  using type = Acc;
};

template <template <typename, typename> typename F,
          TypeListLike List,
          typename Init>
using FoldRight = typename FoldRightImpl<F, List, Init>::type;

//==============================================================================
// Boolean / Value Reductions
//==============================================================================

// AllOf ----------------------------------------------------------------------

template <template <typename> typename Predicate, TypeListLike List>
struct AllOf;

template <template <typename> typename Predicate, typename... Types>
struct AllOf<Predicate, TypeList<Types...>>
    : std::bool_constant<(true && ... && Predicate<Types>::value)> {};

// AnyOf ----------------------------------------------------------------------

template <template <typename> typename Predicate, TypeListLike List>
struct AnyOf;

template <template <typename> typename Predicate, typename... Types>
struct AnyOf<Predicate, TypeList<Types...>>
    : std::bool_constant<(false || ... || Predicate<Types>::value)> {};

// NoneOf ---------------------------------------------------------------------

template <template <typename> typename Predicate, TypeListLike List>
struct NoneOf;

template <template <typename> typename Predicate, typename... Types>
struct NoneOf<Predicate, TypeList<Types...>>
    : std::bool_constant<(true && ... && !Predicate<Types>::value)> {};

// CountIf --------------------------------------------------------------------

template <template <typename> typename Predicate, TypeListLike List>
struct CountIf;

template <template <typename> typename Predicate, typename... Types>
struct CountIf<Predicate, TypeList<Types...>>
    : std::integral_constant<std::size_t,
                             (std::size_t{0u} + ... +
                              (Predicate<Types>::value ? 1u : 0u))> {};
//==============================================================================

}  // namespace base
#endif  // BASE_FUNCTIONAL_TYPE_LIST_H_