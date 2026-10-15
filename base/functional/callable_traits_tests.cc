
#include <type_traits>

#include "base/functional/callable_traits.h"
#include "base/functional/common_types.h"
#include "base/functional/type_list.h"

#include <gtest/gtest.h>

namespace base {

//==============================================================================
// Test Fixtures and Helper Types
//==============================================================================

// Test class for member function pointers
class TestClass {
 public:
  int Method(int, double) { return 0; }
  int ConstMethod(int, double) const { return 0; }
  int VolatileMethod(int, double) volatile { return 0; }
  int ConstVolatileMethod(int, double) const volatile { return 0; }

  int LvalueRefMethod(int, double) & { return 0; }
  int ConstLvalueRefMethod(int, double) const& { return 0; }
  int VolatileLvalueRefMethod(int, double) volatile& { return 0; }
  int ConstVolatileLvalueRefMethod(int, double) const volatile& { return 0; }

  int RvalueRefMethod(int, double) && { return 0; }
  int ConstRvalueRefMethod(int, double) const&& { return 0; }
  int VolatileRvalueRefMethod(int, double) volatile&& { return 0; }
  int ConstVolatileRvalueRefMethod(int, double) const volatile&& { return 0; }

  int NoexceptMethod(int, double) noexcept { return 0; }
  int ConstNoexceptMethod(int, double) const noexcept { return 0; }
};

// Test functions
int TestFunction(int, double) {
  return 0;
}
int TestNoexceptFunction(int, double) noexcept {
  return 0;
}

// Test functors
struct TestFunctor {
  int operator()(int, double) { return 0; }
};

struct TestConstFunctor {
  int operator()(int, double) const { return 0; }
};

struct TestNoexceptFunctor {
  int operator()(int, double) noexcept { return 0; }
};

struct LvalueRefFunctor {
  int operator()(int, double) & { return 0; }
};

struct ConstNoexceptFunctor {
  int operator()(int, double) const noexcept { return 0; }
};

// Generic functor (for compilation test only)
struct GenericFunctor {
  template <typename T>
  int operator()(T /*t*/) {
    return 0;
  }
};

//==============================================================================
// Function Pointer Tests
//==============================================================================

TEST(CallableTraitsTest, FunctionPointer) {
  using Traits = CallableTraits<decltype(&TestFunction)>;

  static_assert(Traits::callable_category == CallableCategory::kFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, NoexceptFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestNoexceptFunction)>;

  static_assert(Traits::callable_category == CallableCategory::kFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(Traits::is_noexcept);
}

TEST(CallableTraitsTest, FunctionReference) {
  using Traits = CallableTraits<decltype(TestFunction)>;

  static_assert(Traits::callable_category == CallableCategory::kFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, NoexceptFunctionReference) {
  using Traits = CallableTraits<decltype(TestNoexceptFunction)>;

  static_assert(Traits::callable_category == CallableCategory::kFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(Traits::is_noexcept);
}

//==============================================================================
// Function Type Tests
//==============================================================================

TEST(CallableTraitsTest, FunctionType) {
  using Traits = CallableTraits<int(int, double)>;

  static_assert(Traits::callable_category == CallableCategory::kFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, NoexceptFunctionType) {
  using Traits = CallableTraits<int(int, double) noexcept>;

  static_assert(Traits::callable_category == CallableCategory::kFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(Traits::is_noexcept);
}

//==============================================================================
// Member Function Pointer Tests
//==============================================================================

TEST(CallableTraitsTest, MemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::Method)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(!Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, ConstMemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::ConstMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, VolatileMemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::VolatileMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(!Traits::is_const);
  static_assert(Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, ConstVolatileMemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::ConstVolatileMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(Traits::is_const);
  static_assert(Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, LvalueRefMemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::LvalueRefMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(!Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, ConstLvalueRefMemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::ConstLvalueRefMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, RvalueRefMemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::RvalueRefMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(!Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, ConstRvalueRefMemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::ConstRvalueRefMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, VolatileLvalueRefMemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::VolatileLvalueRefMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(!Traits::is_const);
  static_assert(Traits::is_volatile);
  static_assert(Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, ConstVolatileLvalueRefMemberFunctionPointer) {
  using Traits =
      CallableTraits<decltype(&TestClass::ConstVolatileLvalueRefMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(Traits::is_const);
  static_assert(Traits::is_volatile);
  static_assert(Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, VolatileRvalueRefMemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::VolatileRvalueRefMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(!Traits::is_const);
  static_assert(Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, ConstVolatileRvalueRefMemberFunctionPointer) {
  using Traits =
      CallableTraits<decltype(&TestClass::ConstVolatileRvalueRefMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(Traits::is_const);
  static_assert(Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, NoexceptMemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::NoexceptMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(!Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(Traits::is_noexcept);
}

TEST(CallableTraitsTest, ConstNoexceptMemberFunctionPointer) {
  using Traits = CallableTraits<decltype(&TestClass::ConstNoexceptMethod)>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
  static_assert(Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(Traits::is_noexcept);
}

// Direct type syntax tests
TEST(CallableTraitsTest, ConstMemberFunctionPointerDirect) {
  using Traits = CallableTraits<int (TestClass::*)(int, double) const>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, NoexceptMemberFunctionPointerDirect) {
  using Traits = CallableTraits<int (TestClass::*)(int, double) noexcept>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(!Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(Traits::is_noexcept);
}

TEST(CallableTraitsTest, ConstNoexceptMemberFunctionPointerDirect) {
  using Traits = CallableTraits<int (TestClass::*)(int, double) const noexcept>;

  static_assert(Traits::callable_category == CallableCategory::kMemFn);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(Traits::is_const);
  static_assert(Traits::is_noexcept);
}

//==============================================================================
// Functor Tests
//==============================================================================

TEST(CallableTraitsTest, Functor) {
  TestFunctor functor;
  using Traits = CallableTraits<decltype(functor)>;

  static_assert(Traits::callable_category == CallableCategory::kFunctor);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(!Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, ConstFunctor) {
  TestConstFunctor functor;
  using Traits = CallableTraits<decltype(functor)>;

  static_assert(Traits::callable_category == CallableCategory::kFunctor);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, NoexceptFunctor) {
  TestNoexceptFunctor functor;
  using Traits = CallableTraits<decltype(functor)>;

  static_assert(Traits::callable_category == CallableCategory::kFunctor);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(!Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(Traits::is_noexcept);
}

TEST(CallableTraitsTest, LvalueRefFunctor) {
  LvalueRefFunctor functor;
  using Traits = CallableTraits<decltype(functor)>;

  static_assert(Traits::callable_category == CallableCategory::kFunctor);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(Traits::is_lv_ref);
}

TEST(CallableTraitsTest, ConstNoexceptFunctor) {
  ConstNoexceptFunctor functor;
  using Traits = CallableTraits<decltype(functor)>;

  static_assert(Traits::callable_category == CallableCategory::kFunctor);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(Traits::is_const);
  static_assert(Traits::is_noexcept);
}

TEST(CallableTraitsTest, GenericFunctorCompilation) {
  // Note: Template operator() is not fully supported by CallableTraits
  // This test verifies that the code compiles
  GenericFunctor functor;
  (void)sizeof(functor);  // Suppress unused warning
}

//==============================================================================
// Lambda Tests
//==============================================================================

TEST(CallableTraitsTest, Lambda) {
  auto lambda = [](int, double) -> int { return 0; };
  using Traits = CallableTraits<decltype(lambda)>;

  static_assert(Traits::callable_category == CallableCategory::kFunctor);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, MutableLambda) {
  auto lambda = [](int, double) mutable -> int { return 0; };
  using Traits = CallableTraits<decltype(lambda)>;

  static_assert(Traits::callable_category == CallableCategory::kFunctor);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(!Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(!Traits::is_noexcept);
}

TEST(CallableTraitsTest, NoexceptLambda) {
  auto lambda = [](int, double) noexcept -> int { return 0; };
  using Traits = CallableTraits<decltype(lambda)>;

  static_assert(Traits::callable_category == CallableCategory::kFunctor);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(std::is_same_v<typename Traits::CanonicalParameterList,
                               TypeList<int, double>>);
  static_assert(Traits::is_const);
  static_assert(!Traits::is_volatile);
  static_assert(!Traits::is_lv_ref);
  static_assert(!Traits::is_rv_ref);
  static_assert(Traits::is_noexcept);
}

TEST(CallableTraitsTest, LambdaWithCapture) {
  int x = 0;
  auto lambda = [x](int a, double) mutable -> int { return a + x; };
  using Traits = CallableTraits<decltype(lambda)>;

  static_assert(Traits::callable_category == CallableCategory::kFunctor);
  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
  static_assert(!Traits::is_const);  // mutable lambda
}

// Note: Generic lambdas are not supported by CallableTraits
// TEST(CallableTraitsTest, GenericLambda) {
//   auto lambda = [](auto...) -> int { return 0; };
//   // This would fail: generic lambdas require template specialization
// }

//==============================================================================
// Return Type Tests
//==============================================================================

TEST(CallableTraitsTest, VoidReturnType) {
  using Traits = CallableTraits<void(int, double)>;

  static_assert(std::is_same_v<typename Traits::Return, void>);
  static_assert(
      std::is_same_v<typename Traits::ParameterList, TypeList<int, double>>);
}

TEST(CallableTraitsTest, PointerReturnType) {
  using Traits = CallableTraits<int*()>;

  static_assert(std::is_same_v<typename Traits::Return, int*>);
}

TEST(CallableTraitsTest, ReferenceReturnType) {
  using Traits = CallableTraits<int&()>;

  static_assert(std::is_same_v<typename Traits::Return, int&>);
}

//==============================================================================
// Parameter Type Tests
//==============================================================================

TEST(CallableTraitsTest, NoParameters) {
  using Traits = CallableTraits<int()>;

  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(std::is_same_v<typename Traits::ParameterList, TypeList<>>);
  static_assert(Traits::ParameterList::size == 0);
}

TEST(CallableTraitsTest, MultipleParameters) {
  using Traits = CallableTraits<int(int, double, char, float)>;

  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(std::is_same_v<typename Traits::ParameterList,
                               TypeList<int, double, char, float>>);
  static_assert(Traits::ParameterList::size == 4);
}

TEST(CallableTraitsTest, ReferenceParameters) {
  using Traits = CallableTraits<int(int&, double const&, char&&)>;

  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(std::is_same_v<typename Traits::ParameterList,
                               TypeList<int&, double const&, char&&>>);
}

TEST(CallableTraitsTest, PointerParameters) {
  using Traits = CallableTraits<int(int*, double const*)>;

  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(std::is_same_v<typename Traits::ParameterList,
                               TypeList<int*, double const*>>);
}

TEST(CallableTraitsTest, ArrayParameter) {
  using Traits = CallableTraits<int(int[])>;

  static_assert(std::is_same_v<typename Traits::Return, int>);
  // Arrays decay to pointers
  static_assert(std::is_same_v<typename Traits::ParameterList, TypeList<int*>>);
}

TEST(CallableTraitsTest, FunctionPointerParameter) {
  using Traits = CallableTraits<int(int (*)(double))>;

  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(std::is_same_v<typename Traits::ParameterList,
                               TypeList<int (*)(double)>>);
}

TEST(CallableTraitsTest, ComplexParameterTypes) {
  using Traits =
      CallableTraits<int(std::pair<int, double>, std::tuple<int, double>)>;

  static_assert(std::is_same_v<typename Traits::Return, int>);
  static_assert(Traits::ParameterList::size == 2);
}

//==============================================================================
// Meta Information Tests
//==============================================================================

TEST(CallableTraitsTest, CallableCategoryValues) {
  static_assert(CallableCategory::kFn == CallableCategory::kFn);
  static_assert(CallableCategory::kMemFn == CallableCategory::kMemFn);
  static_assert(CallableCategory::kFunctor == CallableCategory::kFunctor);
}

TEST(CallableTraitsTest, TypeTraitsIntegration) {
  using FunctionTraits = CallableTraits<int(int, double)>;
  using MemberTraits = CallableTraits<decltype(&TestClass::Method)>;
  using FunctorTraits = CallableTraits<TestFunctor>;

  // Verify return types
  static_assert(std::is_same_v<typename FunctionTraits::Return, int>);
  static_assert(std::is_same_v<typename MemberTraits::Return, int>);
  static_assert(std::is_same_v<typename FunctorTraits::Return, int>);

  // Verify parameter list types
  static_assert(std::is_same_v<typename FunctionTraits::ParameterList,
                               TypeList<int, double>>);
  static_assert(std::is_same_v<typename MemberTraits::ParameterList,
                               TypeList<int, double>>);
  static_assert(std::is_same_v<typename FunctorTraits::ParameterList,
                               TypeList<int, double>>);

  // Verify canonical parameter list for member functions includes 'this'
  static_assert(std::is_same_v<typename MemberTraits::CanonicalParameterList,
                               TypeList<TestClass*, int, double>>);
}

}  // namespace base
