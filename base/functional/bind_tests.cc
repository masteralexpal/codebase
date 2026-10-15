#include "base/functional/bind.h"

#include <gtest/gtest.h>

#include "base/functional/bind_impl.h"
#include "base/functional/callable_traits.h"
#include "base/functional/common_types.h"

#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace base {
namespace {

int Add(int a, int b) {
  return a + b;
}

struct Counter {
  int value = 0;

  int Get() const { return value; }
  void Set(int v) { value = v; }
  int AddTo(int x) { return value += x; }
};

TEST(BindOnce, FreeFunctionFullyBound) {
  auto closure = BindOnce(&Add, 1, 2);
  ASSERT_EQ(std::move(closure).Run(), 3);
}

TEST(BindOnce, PartiallyBoundLeavesUnboundParameters) {
  auto closure = BindOnce(&Add, 1);
  ASSERT_EQ(std::move(closure).Run(2), 3);
}

TEST(BindOnce, Lambda) {
  auto closure = BindOnce([](int a, int b) { return a * b; }, 3);
  ASSERT_EQ(std::move(closure).Run(4), 12);
}

TEST(BindOnce, MoveOnlyBoundArgument) {
  auto closure = BindOnce([](std::unique_ptr<int> p) { return *p; },
                          std::make_unique<int>(42));
  ASSERT_EQ(std::move(closure).Run(), 42);
}

TEST(BindOnce, BoundArgumentsAreStoredByValue) {
  int x = 10;
  auto closure = BindOnce([](int a, int b) { return a + b; }, x, 5);
  x = 100;  // The bound copy must not observe later mutations.
  ASSERT_EQ(std::move(closure).Run(), 15);
}

TEST(BindOnce, MemberFunctionWithReferenceWrapper) {
  Counter counter;
  counter.value = 7;
  auto closure = BindOnce(&Counter::Get, std::ref(counter));
  ASSERT_EQ(std::move(closure).Run(), 7);
}

TEST(BindOnce, MemberFunctionWithSharedPtr) {
  auto counter = std::make_shared<Counter>();
  counter->value = 9;
  auto closure = BindOnce(&Counter::AddTo, counter, 1);
  ASSERT_EQ(std::move(closure).Run(), 10);
  ASSERT_EQ(counter->value, 10);
}

TEST(BindOnce, WeakPtrReceiverRunsWhileAlive) {
  auto counter = std::make_shared<Counter>();
  auto closure = BindOnce(&Counter::Set, std::weak_ptr<Counter>(counter), 42);
  std::move(closure).Run();
  ASSERT_EQ(counter->value, 42);
}

TEST(BindOnce, WeakPtrReceiverIsSkippedWhenExpired) {
  auto counter = std::make_shared<Counter>();
  auto closure =
      BindOnce(&Counter::Set, std::weak_ptr<Counter>(counter), 42);
  counter.reset();
  // The object is gone: the call must be silently skipped, not crash.
  std::move(closure).Run();
}

TEST(BindOnce, ThenChaining) {
  auto closure = BindOnce(&Add, 1, 2).Then([](int x) { return x * 10; });
  ASSERT_EQ(std::move(closure).Run(), 30);
}

TEST(BindRepeating, RunsRepeatedly) {
  auto closure = BindRepeating(&Add, 1);
  ASSERT_EQ(closure.Run(1), 2);
  ASSERT_EQ(closure.Run(2), 3);
}

TEST(BindRepeating, MemberFunctionWithReferenceWrapper) {
  Counter counter;
  auto set = BindRepeating(&Counter::Set, std::ref(counter));
  auto get = BindRepeating(&Counter::Get, std::ref(counter));
  set.Run(100);
  ASSERT_EQ(get.Run(), 100);
}

TEST(BindRepeating, WeakPtrReceiverSkipsAfterExpiry) {
  auto counter = std::make_shared<Counter>();
  auto closure =
      BindRepeating(&Counter::Set, std::weak_ptr<Counter>(counter));
  closure.Run(1);
  ASSERT_EQ(counter->value, 1);
  counter.reset();
  closure.Run(2);  // Silently skipped.
  SUCCEED();
}

// Validator rules that can be asserted positively (the failing bindings
// themselves are hard errors, not SFINAE failures, so they cannot appear
// in negative requires-expressions).
TEST(BindValidator, RejectsMoveOnlyInRepeating) {
  using Traits = CallableTraits<void (*)(std::unique_ptr<int>)>;
  using Validator =
      impl::BindValidator<ClosureCategory::kRepeating, Traits,
                          void (*)(std::unique_ptr<int>), std::unique_ptr<int>>;
  static_assert(!Validator::kNoMoveOnlyInRepeating);
  static_assert(!Validator::value);
}

TEST(BindValidator, RejectsRawPointerReceiver) {
  using Traits = CallableTraits<decltype(&Counter::Get)>;
  using Validator =
      impl::BindValidator<ClosureCategory::kOnce, Traits,
                          decltype(&Counter::Get), Counter*>;
  static_assert(!Validator::kReceiverCheck);
  static_assert(!Validator::value);
}

TEST(BindValidator, RejectsNonVoidWeakPtrCallback) {
  using Traits = CallableTraits<decltype(&Counter::Get)>;
  using Validator =
      impl::BindValidator<ClosureCategory::kOnce, Traits,
                          decltype(&Counter::Get), std::weak_ptr<Counter>>;
  static_assert(!Validator::kStdWeakPtrVoidReturn);
  static_assert(!Validator::value);
}

TEST(BindValidator, RejectsExcessArity) {
  using Traits = CallableTraits<decltype(&Add)>;
  using Validator = impl::BindValidator<ClosureCategory::kOnce, Traits,
                                        decltype(&Add), int, int, int>;
  static_assert(!Validator::kArityOk);
  static_assert(!Validator::value);
}

TEST(BindValidator, AcceptsOrdinaryBindings) {
  using Traits = CallableTraits<decltype(&Add)>;
  using Validator = impl::BindValidator<ClosureCategory::kRepeating, Traits,
                                        decltype(&Add), int>;
  static_assert(Validator::value);
}

}  // namespace
}  // namespace base
