#include "base/functional/bind.h"

#include <gtest/gtest.h>

#include "base/functional/bind_impl.h"
#include "base/functional/callable_traits.h"
#include "base/functional/closure.h"
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

int Sum4(int a, int b, int c, int d) {
  return a + b + c + d;
}

std::string Concat3(std::string const& a, std::string const& b,
                    std::string const& c) {
  return a + b + c;
}

struct Counter {
  int value = 0;

  int Get() const { return value; }
  void Set(int v) { value = v; }
  int AddTo(int x) { return value += x; }
  void AddBoth(int x, int y) { value += x + y; }
};

TEST(BindOnce, FreeFunctionFullyBound) {
  auto closure = BindOnce(&Add, 1, 2);
  ASSERT_EQ(std::move(closure).Run(), 3);
}

TEST(BindOnce, PartiallyBoundLeavesUnboundParameters) {
  auto closure = BindOnce(&Add, 1);
  ASSERT_EQ(std::move(closure).Run(2), 3);
}

TEST(BindOnce, ThreeBoundArguments) {
  auto closure = BindOnce(&Sum4, 1, 2, 3);
  ASSERT_EQ(std::move(closure).Run(4), 10);
}

TEST(BindOnce, BindsStringArguments) {
  auto closure = BindOnce(&Concat3, std::string("a"), std::string("b"));
  ASSERT_EQ(std::move(closure).Run(std::string("c")), "abc");
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

TEST(BindOnce, MoveOnlyBoundArgumentMovedIntoCall) {
  // The bound unique_ptr is consumed by the call itself: after Run, the
  // value has been handed to the callee.
  auto closure =
      BindOnce([](std::unique_ptr<int> p, int x) { return *p + x; },
               std::make_unique<int>(40));
  ASSERT_EQ(std::move(closure).Run(2), 42);
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

TEST(BindOnce, ConstObjectViaCRef) {
  Counter counter;
  counter.value = 7;
  auto closure = BindOnce(&Counter::Get, std::cref(counter));
  ASSERT_EQ(std::move(closure).Run(), 7);
}

TEST(BindOnce, MemberFunctionWithSharedPtr) {
  auto counter = std::make_shared<Counter>();
  counter->value = 9;
  auto closure = BindOnce(&Counter::AddTo, counter, 1);
  ASSERT_EQ(std::move(closure).Run(), 10);
  ASSERT_EQ(counter->value, 10);
}

TEST(BindOnce, MemberFunctionWithSharedPtrKeepsObjectAlive) {
  std::weak_ptr<Counter> weak;
  OnceClosure<int()> closure;
  {
    auto counter = std::make_shared<Counter>();
    counter->value = 5;
    weak = counter;
    closure = BindOnce(&Counter::Get, counter);
    // The closure holds its own shared_ptr copy: the object outlives this
    // scope even after the local is gone.
  }
  ASSERT_FALSE(weak.expired());
  ASSERT_EQ(std::move(closure).Run(), 5);
}

TEST(BindOnce, WeakPtrReceiverRunsWhileAlive) {
  auto counter = std::make_shared<Counter>();
  auto closure = BindOnce(&Counter::Set, std::weak_ptr<Counter>(counter), 42);
  std::move(closure).Run();
  ASSERT_EQ(counter->value, 42);
}

TEST(BindOnce, WeakPtrReceiverWithExtraBoundArgs) {
  // Two extra bound args after the receiver: exercises the 1+J re-indexing
  // in InvokeWithStdWeakPtrImpl (the locked receiver replaces tuple slot 0).
  auto counter = std::make_shared<Counter>();
  auto closure =
      BindOnce(&Counter::AddBoth, std::weak_ptr<Counter>(counter), 40, 2);
  std::move(closure).Run();
  ASSERT_EQ(counter->value, 42);
}

TEST(BindOnce, WeakPtrReceiverWithUnboundParameter) {
  auto counter = std::make_shared<Counter>();
  auto closure = BindOnce(&Counter::Set, std::weak_ptr<Counter>(counter));
  std::move(closure).Run(42);
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

TEST(BindOnce, WeakPtrReceiverWithBoundArgsSkippedWhenExpired) {
  auto counter = std::make_shared<Counter>();
  auto closure =
      BindOnce(&Counter::AddBoth, std::weak_ptr<Counter>(counter), 1, 2);
  counter.reset();
  std::move(closure).Run();
  SUCCEED();
}

TEST(BindOnce, ThenChaining) {
  auto closure = BindOnce(&Add, 1, 2).Then([](int x) { return x * 10; });
  ASSERT_EQ(std::move(closure).Run(), 30);
}

TEST(BindOnce, ExactArityBindingLeavesNoParameters) {
  auto closure = BindOnce(&Sum4, 1, 2, 3, 4);
  static_assert(
      std::is_same_v<decltype(closure), OnceClosure<int()>>);
  ASSERT_EQ(std::move(closure).Run(), 10);
}

TEST(BindRepeating, RunsRepeatedly) {
  auto closure = BindRepeating(&Add, 1);
  ASSERT_EQ(closure.Run(1), 2);
  ASSERT_EQ(closure.Run(2), 3);
}

TEST(BindRepeating, ThreeBoundArguments) {
  auto closure = BindRepeating(&Sum4, 1, 2, 3);
  ASSERT_EQ(closure.Run(4), 10);
  ASSERT_EQ(closure.Run(10), 16);
}

TEST(BindRepeating, CopiesHaveIndependentBoundState) {
  int calls = 0;
  auto first = BindRepeating([&calls](int delta) { calls += delta; }, 1);
  auto second = first;
  first.Run();
  second.Run();
  // Both copies ran; each carried its own copy of the bound 1.
  ASSERT_EQ(calls, 2);
}

TEST(BindRepeating, MemberFunctionWithReferenceWrapper) {
  Counter counter;
  auto set = BindRepeating(&Counter::Set, std::ref(counter));
  auto get = BindRepeating(&Counter::Get, std::ref(counter));
  set.Run(100);
  ASSERT_EQ(get.Run(), 100);
}

TEST(BindRepeating, MemberFunctionWithSharedPtr) {
  auto counter = std::make_shared<Counter>();
  auto add = BindRepeating(&Counter::AddTo, counter);
  ASSERT_EQ(add.Run(1), 1);
  ASSERT_EQ(add.Run(2), 3);
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

TEST(BindRepeating, WeakPtrReceiverWithBoundArgs) {
  auto counter = std::make_shared<Counter>();
  auto closure =
      BindRepeating(&Counter::AddBoth, std::weak_ptr<Counter>(counter), 1);
  closure.Run(10);
  ASSERT_EQ(counter->value, 11);
  closure.Run(10);
  ASSERT_EQ(counter->value, 22);
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

TEST(BindValidator, RejectsByValueReceiver) {
  // A by-value receiver would silently slice a copy of the object into the
  // closure; only wrappers that say something about lifetime are accepted.
  using Traits = CallableTraits<decltype(&Counter::Get)>;
  using Validator =
      impl::BindValidator<ClosureCategory::kOnce, Traits,
                          decltype(&Counter::Get), Counter>;
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

TEST(BindValidator, AcceptsExactArityBinding) {
  using Traits = CallableTraits<decltype(&Add)>;
  using Validator = impl::BindValidator<ClosureCategory::kOnce, Traits,
                                        decltype(&Add), int, int>;
  static_assert(Validator::kArityOk);
  static_assert(Validator::value);
}

TEST(BindValidator, AcceptsSharedPtrReceiver) {
  using Traits = CallableTraits<decltype(&Counter::AddTo)>;
  using Validator =
      impl::BindValidator<ClosureCategory::kOnce, Traits,
                          decltype(&Counter::AddTo), std::shared_ptr<Counter>>;
  static_assert(Validator::kReceiverCheck);
  static_assert(Validator::value);
}

TEST(BindValidator, AcceptsMoveOnlyInOnce) {
  using Traits = CallableTraits<void (*)(std::unique_ptr<int>)>;
  using Validator =
      impl::BindValidator<ClosureCategory::kOnce, Traits,
                          void (*)(std::unique_ptr<int>), std::unique_ptr<int>>;
  static_assert(Validator::kNoMoveOnlyInRepeating);
  static_assert(Validator::value);
}

void WriteInto(int& out, int v) {
  out = v;
}

int CStrLength(char const* s) {
  int n = 0;
  while (s[n] != '\0') {
    ++n;
  }
  return n;
}

int FortyTwo() {
  return 42;
}

TEST(BindOnce, BoundReferenceWrapperMutatesOriginal) {
  // Unwrap applies to every bound arg, not just member receivers: a bound
  // std::ref arrives as a real reference at the call.
  int x = 0;
  auto closure = BindOnce(&WriteInto, std::ref(x), 42);
  std::move(closure).Run();
  ASSERT_EQ(x, 42);
}

TEST(BindOnce, PartialBindingKeepsTrailingParameterOrder) {
  auto closure = BindOnce(&Sum4, 1, 2);
  ASSERT_EQ(std::move(closure).Run(3, 4), 10);
  auto concat = BindOnce(&Concat3, std::string("a"));
  ASSERT_EQ(std::move(concat).Run(std::string("b"), std::string("c")), "abc");
}

TEST(BindOnce, BoundStringLiteralDecaysToPointer) {
  auto closure = BindOnce(&CStrLength, "hello");
  ASSERT_EQ(std::move(closure).Run(), 5);
}

TEST(BindOnce, ZeroArgumentCallable) {
  auto closure = BindOnce(&FortyTwo);
  ASSERT_EQ(std::move(closure).Run(), 42);
}

TEST(BindOnce, SharedPtrToConstObject) {
  auto counter = std::make_shared<Counter>();
  counter->value = 42;
  std::shared_ptr<Counter const> const_counter = counter;
  auto closure = BindOnce(&Counter::Get, const_counter);
  ASSERT_EQ(std::move(closure).Run(), 42);
}

TEST(BindOnce, WeakPtrReceiverThenChaining) {
  auto counter = std::make_shared<Counter>();
  auto closure = BindOnce(&Counter::Set, std::weak_ptr<Counter>(counter), 42)
                     .Then([] { return 7; });
  ASSERT_EQ(std::move(closure).Run(), 7);
  ASSERT_EQ(counter->value, 42);
}

TEST(BindOnce, WeakPtrExpiredThenStillRunsNextStage) {
  auto counter = std::make_shared<Counter>();
  auto closure = BindOnce(&Counter::Set, std::weak_ptr<Counter>(counter), 42)
                     .Then([] { return 7; });
  counter.reset();
  // The weak stage is skipped, but Then's continuation still runs.
  ASSERT_EQ(std::move(closure).Run(), 7);
}

TEST(BindRepeating, BoundByValueArgSnapshottedPerCopy) {
  // A bound by-value argument is copied into the closure; copies of the
  // closure each carry the snapshot as of construction time.
  Counter counter;
  counter.value = 10;
  auto first = BindRepeating([](Counter c) { return c.value; }, counter);
  counter.value = 99;
  auto second = first;
  ASSERT_EQ(first.Run(), 10);
  ASSERT_EQ(second.Run(), 10);
}

TEST(BindRepeating, BindsAndRunsZeroArgumentCallable) {
  auto closure = BindRepeating(&FortyTwo);
  ASSERT_EQ(closure.Run(), 42);
  ASSERT_EQ(closure.Run(), 42);
}

TEST(BindOnce, StressManyBoundClosures) {
  // 5k bound closures through construction, storage, and a single
  // consuming call: ASan food for the tuple storage and invoke paths.
  long long sum = 0;
  for (int i = 0; i < 5000; ++i) {
    sum += BindOnce(&Sum4, i, 1, 1).Run(i);
  }
  ASSERT_EQ(sum, 25005000LL);
}

}  // namespace
}  // namespace base
