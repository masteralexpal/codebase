
#include "base/functional/type_list.h"

#include <string>
#include <type_traits>

#include <gtest/gtest.h>

namespace base {

//==============================================================================
// Test Helper Templates
//==============================================================================

namespace test {

template <typename T>
struct AddConst {
  using type = T const;
};

template <typename T>
struct AddLValueRef {
  using type = T&;
};

template <typename T>
struct AddRValueRef {
  using type = T&&;
};

template <typename T>
struct IsIntegral {
  static constexpr bool value = std::is_integral_v<T>;
};

template <typename T>
struct IsFloatingPoint {
  static constexpr bool value = std::is_floating_point_v<T>;
};

template <typename T>
struct IsString {
  static constexpr bool value = std::is_same_v<T, std::string>;
};

template <typename T>
struct IsPointer {
  static constexpr bool value = std::is_pointer_v<T>;
};

template <typename Acc, typename T>
struct ConcatTypesLeft {
  using type = Prepend<Acc, T>;
};

template <typename T, typename Acc>
struct ConcatTypesRight {
  using type = Prepend<Acc, T>;
};

template <typename T>
struct DoubleValue {
  static constexpr int value = T::value * 2;
};

}  // namespace test

//==============================================================================
// TypeList Basic Tests
//==============================================================================

TEST(TypeListTest, EmptyList) {
  using EmptyList = TypeList<>;
  static_assert(EmptyList::size == 0);
  static_assert(IsTypeList<EmptyList>::value);
}

TEST(TypeListTest, SingleElement) {
  using SingleList = TypeList<int>;
  static_assert(SingleList::size == 1);
  static_assert(std::is_same_v<At<SingleList, 0>, int>);
}

TEST(TypeListTest, MultipleElements) {
  using MixedList = TypeList<int, double, std::string>;
  static_assert(MixedList::size == 3);
  static_assert(std::is_same_v<At<MixedList, 0>, int>);
  static_assert(std::is_same_v<At<MixedList, 1>, double>);
  static_assert(std::is_same_v<At<MixedList, 2>, std::string>);
}

TEST(TypeListTest, IsTypeListTrait) {
  static_assert(IsTypeList<TypeList<>>::value);
  static_assert(IsTypeList<TypeList<int>>::value);
  static_assert(IsTypeList<TypeList<int, double>>::value);

  static_assert(!IsTypeList<int>::value);
  static_assert(!IsTypeList<std::tuple<>>::value);
  static_assert(!IsTypeList<void>::value);
}

//==============================================================================
// Prepend Tests
//==============================================================================

TEST(TypeListTest, PrependToEmpty) {
  using EmptyList = TypeList<>;
  using Result = Prepend<EmptyList, int>;
  static_assert(Result::size == 1);
  static_assert(std::is_same_v<At<Result, 0>, int>);
}

TEST(TypeListTest, PrependToNonEmpty) {
  using IntList = TypeList<int>;
  using Result = Prepend<IntList, double>;
  static_assert(Result::size == 2);
  static_assert(std::is_same_v<At<Result, 0>, double>);
  static_assert(std::is_same_v<At<Result, 1>, int>);
}

TEST(TypeListTest, PrependMultiple) {
  using List = TypeList<int, double>;
  using Result = Prepend<Prepend<List, std::string>, bool>;
  static_assert(Result::size == 4);
  static_assert(std::is_same_v<At<Result, 0>, bool>);
  static_assert(std::is_same_v<At<Result, 1>, std::string>);
  static_assert(std::is_same_v<At<Result, 2>, int>);
  static_assert(std::is_same_v<At<Result, 3>, double>);
}

//==============================================================================
// Concat Tests
//==============================================================================

TEST(TypeListTest, ConcatEmpty) {
  using Result = Concat<TypeList<>, TypeList<>>;
  static_assert(Result::size == 0);
}

TEST(TypeListTest, ConcatWithEmpty) {
  using IntList = TypeList<int, double>;

  using Result1 = Concat<TypeList<>, IntList>;
  static_assert(Result1::size == 2);
  static_assert(std::is_same_v<At<Result1, 0>, int>);

  using Result2 = Concat<IntList, TypeList<>>;
  static_assert(Result2::size == 2);
  static_assert(std::is_same_v<At<Result2, 1>, double>);
}

TEST(TypeListTest, ConcatNonEmpty) {
  using List1 = TypeList<int, double>;
  using List2 = TypeList<std::string, bool>;
  using Result = Concat<List1, List2>;
  static_assert(Result::size == 4);
  static_assert(std::is_same_v<At<Result, 0>, int>);
  static_assert(std::is_same_v<At<Result, 1>, double>);
  static_assert(std::is_same_v<At<Result, 2>, std::string>);
  static_assert(std::is_same_v<At<Result, 3>, bool>);
}

//==============================================================================
// Drop Tests
//==============================================================================

TEST(TypeListTest, DropZero) {
  using List = TypeList<int, double, std::string>;
  using Result = Drop<List, 0>;
  static_assert(Result::size == 3);
  static_assert(std::is_same_v<At<Result, 0>, int>);
}

TEST(TypeListTest, DropFromEmpty) {
  using Result = Drop<TypeList<>, 0>;
  static_assert(Result::size == 0);
}

TEST(TypeListTest, DropPartial) {
  using List = TypeList<int, double, std::string, bool>;

  using Drop1 = Drop<List, 1>;
  static_assert(Drop1::size == 3);
  static_assert(std::is_same_v<At<Drop1, 0>, double>);

  using Drop2 = Drop<List, 2>;
  static_assert(Drop2::size == 2);
  static_assert(std::is_same_v<At<Drop2, 0>, std::string>);

  using Drop3 = Drop<List, 3>;
  static_assert(Drop3::size == 1);
  static_assert(std::is_same_v<At<Drop3, 0>, bool>);
}

TEST(TypeListTest, DropAll) {
  using List = TypeList<int, double, std::string>;
  using Result = Drop<List, 3>;
  static_assert(Result::size == 0);
}

//==============================================================================
// Take Tests
//==============================================================================

TEST(TypeListTest, TakeZero) {
  using List = TypeList<int, double, std::string>;
  using Result = Take<List, 0>;
  static_assert(Result::size == 0);
}

TEST(TypeListTest, TakeFromEmpty) {
  using Result = Take<TypeList<>, 0>;
  static_assert(Result::size == 0);
}

TEST(TypeListTest, TakePartial) {
  using List = TypeList<int, double, std::string, bool>;

  using Take1 = Take<List, 1>;
  static_assert(Take1::size == 1);
  static_assert(std::is_same_v<At<Take1, 0>, int>);

  using Take2 = Take<List, 2>;
  static_assert(Take2::size == 2);
  static_assert(std::is_same_v<At<Take2, 1>, double>);

  using Take3 = Take<List, 3>;
  static_assert(Take3::size == 3);
  static_assert(std::is_same_v<At<Take3, 2>, std::string>);
}

TEST(TypeListTest, TakeAll) {
  using List = TypeList<int, double, std::string>;
  using Result = Take<List, 3>;
  static_assert(Result::size == 3);
  static_assert(std::is_same_v<At<Result, 2>, std::string>);
}

//==============================================================================
// At Tests
//==============================================================================

TEST(TypeListTest, AtFirst) {
  using List = TypeList<int, double, std::string>;
  static_assert(std::is_same_v<At<List, 0>, int>);
}

TEST(TypeListTest, AtMiddle) {
  using List = TypeList<int, double, std::string, bool>;
  static_assert(std::is_same_v<At<List, 2>, std::string>);
}

TEST(TypeListTest, AtLast) {
  using List = TypeList<int, double, std::string>;
  static_assert(std::is_same_v<At<List, 2>, std::string>);
}

TEST(TypeListTest, AtWithVariousTypes) {
  using List = TypeList<int, double*, std::string&, bool(int)>;
  static_assert(std::is_same_v<At<List, 0>, int>);
  static_assert(std::is_same_v<At<List, 1>, double*>);
  static_assert(std::is_same_v<At<List, 2>, std::string&>);
  static_assert(std::is_same_v<At<List, 3>, bool(int)>);
}

//==============================================================================
// Replace Tests
//==============================================================================

TEST(TypeListTest, ReplaceFirst) {
  using List = TypeList<int, double, std::string>;
  using Result = Replace<List, 0, float>;
  static_assert(Result::size == 3);
  static_assert(std::is_same_v<At<Result, 0>, float>);
  static_assert(std::is_same_v<At<Result, 1>, double>);
}

TEST(TypeListTest, ReplaceMiddle) {
  using List = TypeList<int, double, std::string>;
  using Result = Replace<List, 1, bool>;
  static_assert(Result::size == 3);
  static_assert(std::is_same_v<At<Result, 0>, int>);
  static_assert(std::is_same_v<At<Result, 1>, bool>);
  static_assert(std::is_same_v<At<Result, 2>, std::string>);
}

TEST(TypeListTest, ReplaceLast) {
  using List = TypeList<int, double, std::string>;
  using Result = Replace<List, 2, char>;
  static_assert(Result::size == 3);
  static_assert(std::is_same_v<At<Result, 2>, char>);
}

TEST(TypeListTest, ReplaceWithComplexType) {
  using List = TypeList<int, double>;
  using Result = Replace<List, 0, std::vector<std::string>>;
  static_assert(Result::size == 2);
  static_assert(std::is_same_v<At<Result, 0>, std::vector<std::string>>);
}

//==============================================================================
// Map Tests
//==============================================================================

TEST(TypeListTest, MapAddConst) {
  using List = TypeList<int, double, std::string>;
  using Result = Map<test::AddConst, List>;
  static_assert(Result::size == 3);
  static_assert(std::is_same_v<At<Result, 0>, int const>);
  static_assert(std::is_same_v<At<Result, 1>, double const>);
  static_assert(std::is_same_v<At<Result, 2>, std::string const>);
}

TEST(TypeListTest, MapAddLValueRef) {
  using List = TypeList<int, double>;
  using Result = Map<test::AddLValueRef, List>;
  static_assert(Result::size == 2);
  static_assert(std::is_same_v<At<Result, 0>, int&>);
  static_assert(std::is_same_v<At<Result, 1>, double&>);
}

TEST(TypeListTest, MapAddRValueRef) {
  using List = TypeList<int, double>;
  using Result = Map<test::AddRValueRef, List>;
  static_assert(Result::size == 2);
  static_assert(std::is_same_v<At<Result, 0>, int&&>);
  static_assert(std::is_same_v<At<Result, 1>, double&&>);
}

TEST(TypeListTest, MapEmptyList) {
  using Result = Map<test::AddConst, TypeList<>>;
  static_assert(Result::size == 0);
}

//==============================================================================
// Filter Tests
//==============================================================================

TEST(TypeListTest, FilterIntegral) {
  using List = TypeList<int, double, bool, std::string, char>;
  using Result = Filter<test::IsIntegral, List>;
  static_assert(Result::size == 3);
  static_assert(std::is_same_v<At<Result, 0>, int>);
  static_assert(std::is_same_v<At<Result, 1>, bool>);
  static_assert(std::is_same_v<At<Result, 2>, char>);
}

TEST(TypeListTest, FilterFloatingPoint) {
  using List = TypeList<int, double, bool, std::string>;
  using Result = Filter<test::IsFloatingPoint, List>;
  static_assert(Result::size == 1);
  static_assert(std::is_same_v<At<Result, 0>, double>);
}

TEST(TypeListTest, FilterString) {
  using List = TypeList<int, double, std::string, bool>;
  using Result = Filter<test::IsString, List>;
  static_assert(Result::size == 1);
  static_assert(std::is_same_v<At<Result, 0>, std::string>);
}

TEST(TypeListTest, FilterNoMatch) {
  using List = TypeList<int, bool, char>;
  using Result = Filter<test::IsFloatingPoint, List>;
  static_assert(Result::size == 0);
}

TEST(TypeListTest, FilterAllMatch) {
  using List = TypeList<int, bool, char>;
  using Result = Filter<test::IsIntegral, List>;
  static_assert(Result::size == 3);
}

TEST(TypeListTest, FilterEmptyList) {
  using Result = Filter<test::IsIntegral, TypeList<>>;
  static_assert(Result::size == 0);
}

//==============================================================================
// FoldLeft Tests
//==============================================================================

TEST(TypeListTest, FoldLeftBasic) {
  using List = TypeList<int, double, std::string>;
  using Result = FoldLeft<test::ConcatTypesLeft, TypeList<>, List>;
  static_assert(Result::size == 3);
  // FoldLeft reverses the order
  static_assert(std::is_same_v<At<Result, 0>, std::string>);
  static_assert(std::is_same_v<At<Result, 1>, double>);
  static_assert(std::is_same_v<At<Result, 2>, int>);
}

TEST(TypeListTest, FoldLeftWithInitialValue) {
  using List = TypeList<int, double>;
  using Initial = TypeList<bool>;
  using Result = FoldLeft<test::ConcatTypesLeft, Initial, List>;
  static_assert(Result::size == 3);
  // FoldLeft prepends each element, reversing the order
  static_assert(std::is_same_v<At<Result, 0>, double>);
  static_assert(std::is_same_v<At<Result, 1>, int>);
  static_assert(std::is_same_v<At<Result, 2>, bool>);
}

TEST(TypeListTest, FoldLeftEmptyList) {
  using Initial = TypeList<int>;
  using Result = FoldLeft<test::ConcatTypesLeft, Initial, TypeList<>>;
  static_assert(Result::size == 1);
  static_assert(std::is_same_v<At<Result, 0>, int>);
}

//==============================================================================
// FoldRight Tests
//==============================================================================

TEST(TypeListTest, FoldRightBasic) {
  using List = TypeList<int, double, std::string>;
  using Result = FoldRight<test::ConcatTypesRight, List, TypeList<>>;
  static_assert(Result::size == 3);
  // FoldRight preserves the order
  static_assert(std::is_same_v<At<Result, 0>, int>);
  static_assert(std::is_same_v<At<Result, 1>, double>);
  static_assert(std::is_same_v<At<Result, 2>, std::string>);
}

TEST(TypeListTest, FoldRightEmptyList) {
  using Initial = TypeList<int>;
  using Result = FoldRight<test::ConcatTypesRight, TypeList<>, Initial>;
  static_assert(Result::size == 1);
  static_assert(std::is_same_v<At<Result, 0>, int>);
}

//==============================================================================
// Boolean Reduction Tests
//==============================================================================

TEST(TypeListTest, AllOf) {
  using IntegralList = TypeList<int, bool, char>;
  static_assert(AllOf<test::IsIntegral, IntegralList>::value);

  using MixedList = TypeList<int, double, bool>;
  static_assert(!AllOf<test::IsIntegral, MixedList>::value);

  // Empty list: all predicates are vacuously true
  static_assert(AllOf<test::IsIntegral, TypeList<>>::value);
}

TEST(TypeListTest, AnyOf) {
  using MixedList = TypeList<int, double, bool>;
  static_assert(AnyOf<test::IsFloatingPoint, MixedList>::value);

  using IntegralList = TypeList<int, bool, char>;
  static_assert(!AnyOf<test::IsFloatingPoint, IntegralList>::value);

  // Empty list: no elements satisfy any predicate
  static_assert(!AnyOf<test::IsIntegral, TypeList<>>::value);
}

TEST(TypeListTest, NoneOf) {
  using IntegralList = TypeList<int, bool, char>;
  static_assert(NoneOf<test::IsFloatingPoint, IntegralList>::value);

  using MixedList = TypeList<int, double, bool>;
  static_assert(!NoneOf<test::IsFloatingPoint, MixedList>::value);

  // Empty list: no elements violate any predicate
  static_assert(NoneOf<test::IsIntegral, TypeList<>>::value);
}

//==============================================================================
// CountIf Tests
//==============================================================================

TEST(TypeListTest, CountIfBasic) {
  using List = TypeList<int, double, bool, std::string, char>;
  static_assert(CountIf<test::IsIntegral, List>::value == 3);
  static_assert(CountIf<test::IsFloatingPoint, List>::value == 1);
  static_assert(CountIf<test::IsString, List>::value == 1);
}

TEST(TypeListTest, CountIfNoMatch) {
  using List = TypeList<int, bool, char>;
  static_assert(CountIf<test::IsFloatingPoint, List>::value == 0);
}

TEST(TypeListTest, CountIfAllMatch) {
  using List = TypeList<int, bool, char>;
  static_assert(CountIf<test::IsIntegral, List>::value == 3);
}

TEST(TypeListTest, CountIfEmptyList) {
  static_assert(CountIf<test::IsIntegral, TypeList<>>::value == 0);
}

//==============================================================================
// Complex Type Tests
//==============================================================================

TEST(TypeListTest, ComplexTypes) {
  using ComplexList = TypeList<int*, double&, std::string&&, bool(int, double),
                               void (*)(float)>;

  static_assert(ComplexList::size == 5);
  static_assert(std::is_pointer_v<At<ComplexList, 0>>);
  static_assert(std::is_reference_v<At<ComplexList, 1>>);
  static_assert(std::is_function_v<At<ComplexList, 3>>);
}

TEST(TypeListTest, CVQualifiers) {
  using CVList =
      TypeList<int const, double volatile, std::string const volatile>;

  static_assert(CVList::size == 3);
  static_assert(std::is_const_v<At<CVList, 0>>);
  static_assert(std::is_volatile_v<At<CVList, 1>>);
  static_assert(std::is_const_v<At<CVList, 2>>);
  static_assert(std::is_volatile_v<At<CVList, 2>>);
}

//==============================================================================
// Edge Case Tests
//==============================================================================

TEST(TypeListTest, EdgeCases) {
  // Operations on empty lists
  static_assert(Drop<TypeList<>, 0>::size == 0);
  static_assert(Take<TypeList<>, 0>::size == 0);
  static_assert(Concat<TypeList<>, TypeList<>>::size == 0);

  // Single element list
  using SingleList = TypeList<int>;
  static_assert(Drop<SingleList, 1>::size == 0);
  static_assert(Take<SingleList, 1>::size == 1);
  static_assert(std::is_same_v<At<SingleList, 0>, int>);

  // Replace on single element
  using Replaced = Replace<SingleList, 0, double>;
  static_assert(std::is_same_v<At<Replaced, 0>, double>);
}

TEST(TypeListTest, LargeList) {
  using LargeList = TypeList<int, int, int, int, int, int, int, int, int, int,
                             int, int, int, int, int, int, int, int, int, int>;
  static_assert(LargeList::size == 20);
}

//==============================================================================
// Compile-Time Constraint Tests
// These tests verify that the type-level constraints are correctly enforced.
// Out-of-bounds access is prevented by `requires` clauses and will fail to
// compile. These tests document the expected behavior.
//==============================================================================

TEST(TypeListTest, DropConstraintExactlyAtBoundary) {
  using List = TypeList<int, double, std::string>;

  // Drop exactly at boundary: should produce empty list
  using DropAll = Drop<List, 3>;
  static_assert(DropAll::size == 0);

  // Drop zero: should produce same list
  using DropZero = Drop<List, 0>;
  static_assert(DropZero::size == 3);
  static_assert(std::is_same_v<At<DropZero, 0>, int>);
}

TEST(TypeListTest, TakeConstraintExactlyAtBoundary) {
  using List = TypeList<int, double, std::string>;

  // Take exactly at boundary: should produce same list
  using TakeAll = Take<List, 3>;
  static_assert(TakeAll::size == 3);
  static_assert(std::is_same_v<At<TakeAll, 2>, std::string>);

  // Take zero: should produce empty list
  using TakeZero = Take<List, 0>;
  static_assert(TakeZero::size == 0);
}

TEST(TypeListTest, AtBoundaryElements) {
  using List = TypeList<int, double, std::string>;

  // First element
  static_assert(std::is_same_v<At<List, 0>, int>);
  // Last element (index = size - 1)
  static_assert(std::is_same_v<At<List, 2>, std::string>);
}

// Note: The following would fail to compile due to `requires` constraints:
//
// using BadDrop = Drop<TypeList<int>, 2>;  // N > List::size
// using BadTake = Take<TypeList<int>, 2>;  // N > List::size
// using BadAt = At<TypeList<int>, 1>;      // N >= List::size
//
// This is intentional - the type system prevents out-of-bounds access at
// compile time, ensuring type safety without runtime overhead.

}  // namespace base
