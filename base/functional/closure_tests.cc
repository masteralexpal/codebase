#include "base/functional/closure.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace base {
namespace {

int Add(int a, int b) {
  return a + b;
}

struct LifecycleCounter {
  static inline int live = 0;

  LifecycleCounter() { ++live; }
  LifecycleCounter(LifecycleCounter const&) { ++live; }
  LifecycleCounter(LifecycleCounter&&) noexcept { ++live; }
  ~LifecycleCounter() { --live; }

  void operator()() {}
};

TEST(OnceClosure, RunsFreeFunction) {
  OnceClosure<int()> closure([] { return Add(1, 2); });
  ASSERT_EQ(std::move(closure).Run(), 3);
}

TEST(OnceClosure, RunsWithParameters) {
  OnceClosure<int(int, int)> closure(
      [](int a, int b) { return a + b; });
  ASSERT_EQ(std::move(closure).Run(1, 2), 3);
}

TEST(OnceClosure, ForwardsMoveOnlyParameter) {
  OnceClosure<int(std::unique_ptr<int>)> closure(
      [](std::unique_ptr<int> p) { return *p; });
  ASSERT_EQ(std::move(closure).Run(std::make_unique<int>(42)), 42);
}

TEST(OnceClosure, HoldsMoveOnlyCapture) {
  OnceClosure<int()> closure(
      [ptr = std::make_unique<int>(42)] { return *ptr; });
  ASSERT_EQ(std::move(closure).Run(), 42);
}

TEST(OnceClosure, HoldsLargeCapture) {
  // Exceeds the engine's inline buffer: the heap path, through the closure.
  struct Big {
    std::byte payload[128];
    int value = 42;
  };
  OnceClosure<int()> closure([big = Big{}] { return big.value; });
  ASSERT_EQ(std::move(closure).Run(), 42);
}

TEST(OnceClosure, DefaultConstructedIsNull) {
  OnceClosure<void()> closure;
  ASSERT_TRUE(closure.is_null());
  ASSERT_FALSE(static_cast<bool>(closure));
}

TEST(OnceClosure, NotCopyable) {
  static_assert(!std::is_copy_constructible_v<OnceClosure<void()>>);
  static_assert(!std::is_copy_assignable_v<OnceClosure<void()>>);
  static_assert(
      !std::is_constructible_v<OnceClosure<void()>, std::nullptr_t>);
}

TEST(OnceClosure, RunConsumes) {
  int count = 0;
  OnceClosure<void()> closure([&count] { ++count; });
  ASSERT_TRUE(static_cast<bool>(closure));
  std::move(closure).Run();
  ASSERT_FALSE(static_cast<bool>(closure));
  ASSERT_TRUE(closure.is_null());
  ASSERT_EQ(count, 1);
}

TEST(OnceClosure, RvalueRunCompiles) {
  // The lvalue overload of Run() is deleted (consuming must be spelled
  // std::move(closure).Run()). A deleted-function call is a hard error,
  // not a substitution failure, so this property cannot be verified
  // negatively in a requires-expression — only the positive side can:
  static_assert(requires(OnceClosure<void()>&& c) { std::move(c).Run(); });
}

TEST(OnceClosure, MoveConstructor) {
  int count = 0;
  OnceClosure<void()> first([&count] { ++count; });
  OnceClosure<void()> second(std::move(first));
  ASSERT_FALSE(static_cast<bool>(first));
  ASSERT_TRUE(static_cast<bool>(second));
  std::move(second).Run();
  ASSERT_EQ(count, 1);
}

TEST(OnceClosure, MoveAssignment) {
  int count1 = 0;
  int count2 = 0;
  OnceClosure<void()> first([&count1] { ++count1; });
  OnceClosure<void()> second([&count2] { ++count2; });
  second = std::move(first);
  ASSERT_FALSE(static_cast<bool>(first));
  ASSERT_TRUE(static_cast<bool>(second));
  std::move(second).Run();
  ASSERT_EQ(count1, 1);
  ASSERT_EQ(count2, 0);
}

TEST(OnceClosure, MoveAssignmentDestroysPreviousContent) {
  LifecycleCounter::live = 0;
  {
    OnceClosure<void()> doomed =
        OnceClosure<void()>(LifecycleCounter{});
    ASSERT_EQ(LifecycleCounter::live, 1);
    doomed = OnceClosure<void()>([] {});
    ASSERT_EQ(LifecycleCounter::live, 0);
  }
}

TEST(OnceClosure, SelfMoveAssignment) {
  int count = 0;
  OnceClosure<void()> closure([&count] { ++count; });
  OnceClosure<void()>& alias = closure;
  closure = std::move(alias);
  ASSERT_TRUE(static_cast<bool>(closure));
  std::move(closure).Run();
  ASSERT_EQ(count, 1);
}

TEST(OnceClosure, Reset) {
  OnceClosure<void()> closure([] {});
  ASSERT_FALSE(closure.is_null());
  closure.Reset();
  ASSERT_TRUE(closure.is_null());
}

TEST(OnceClosure, ResetAfterConsumptionIsHarmless) {
  OnceClosure<void()> closure([] {});
  std::move(closure).Run();
  closure.Reset();
  ASSERT_TRUE(closure.is_null());
}

TEST(OnceClosure, ThenChainsReturnValues) {
  bool step1_called = false;
  auto closure = OnceClosure<int()>([&step1_called]() -> int {
                   step1_called = true;
                   return 10;
                 }).Then([](int x) { return x * 2; })
                     .Then([](int x) { return x + 5; });
  ASSERT_EQ(std::move(closure).Run(), 25);
  ASSERT_TRUE(step1_called);
}

TEST(OnceClosure, ThenAfterVoid) {
  int order = 0;
  int first_ran = 0;
  auto closure = OnceClosure<void()>([&] { first_ran = ++order; })
                     .Then([&] { return ++order; });
  ASSERT_EQ(std::move(closure).Run(), 2);
  ASSERT_EQ(first_ran, 1);
}

TEST(OnceClosure, ThenToVoidDiscardsValue) {
  bool next_ran = false;
  auto closure =
      OnceClosure<int()>([] { return 42; }).Then([&next_ran](int x) {
        ASSERT_EQ(x, 42);
        next_ran = true;
      });
  std::move(closure).Run();
  ASSERT_TRUE(next_ran);
}

TEST(OnceClosure, ThenMovesMoveOnlyValuesThrough) {
  auto closure = OnceClosure<std::unique_ptr<int>()>(
                     [] { return std::make_unique<int>(42); })
                     .Then([](std::unique_ptr<int> p) { return *p + 1; });
  ASSERT_EQ(std::move(closure).Run(), 43);
}

TEST(OnceClosure, ThenChainIsItselfConsumedOnce) {
  int count = 0;
  auto closure =
      OnceClosure<void()>([] {}).Then([&count] { ++count; });
  std::move(closure).Run();
  ASSERT_TRUE(closure.is_null());
  ASSERT_EQ(count, 1);
}

TEST(RepeatingClosure, RunsRepeatedly) {
  RepeatingClosure<int(int, int)> closure(&Add);
  ASSERT_EQ(closure.Run(1, 2), 3);
  ASSERT_EQ(closure.Run(3, 4), 7);
}

TEST(RepeatingClosure, CopiesShareNothing) {
  int count = 0;
  RepeatingClosure<void()> first([&count] { ++count; });
  RepeatingClosure<void()> second = first;
  first.Run();
  second.Run();
  ASSERT_EQ(count, 2);
}

TEST(RepeatingClosure, MoveConstructorVacatesSource) {
  int count = 0;
  RepeatingClosure<void()> first([&count] { ++count; });
  RepeatingClosure<void()> second(std::move(first));
  ASSERT_FALSE(static_cast<bool>(first));
  second.Run();
  ASSERT_EQ(count, 1);
}

TEST(RepeatingClosure, RvalueRunConsumes) {
  int count = 0;
  RepeatingClosure<void()> closure([&count] { ++count; });
  std::move(closure).Run();
  ASSERT_EQ(count, 1);
  ASSERT_TRUE(closure.is_null());
}

TEST(RepeatingClosure, LvalueThenLeavesOriginalRunnable) {
  RepeatingClosure<int()> original([] { return 10; });
  auto chained = original.Then([](int x) { return x * 2; });
  // The lvalue overload copies: both closures stay alive.
  ASSERT_EQ(original.Run(), 10);
  ASSERT_EQ(chained.Run(), 20);
  ASSERT_EQ(chained.Run(), 20);
}

TEST(RepeatingClosure, ThenOnRvalue) {
  int call_count = 0;
  auto closure = RepeatingClosure<int()>([] { return 1; })
                     .Then([&call_count](int x) {
                       ++call_count;
                       return x + 10;
                     });
  ASSERT_EQ(closure.Run(), 11);
  ASSERT_EQ(closure.Run(), 11);
  ASSERT_EQ(call_count, 2);
}

TEST(RepeatingClosure, ThenAfterVoid) {
  int order = 0;
  auto closure = RepeatingClosure<void()>([&] { ++order; })
                     .Then([&] { order *= 10; });
  closure.Run();
  ASSERT_EQ(order, 10);
  closure.Run();
  ASSERT_EQ(order, 110);
}

TEST(RepeatingClosure, ConvertsToOnceByConsumption) {
  RepeatingClosure<int()> repeating([] { return 42; });
  OnceClosure<int()> once = std::move(repeating);
  ASSERT_FALSE(static_cast<bool>(repeating));
  ASSERT_TRUE(static_cast<bool>(once));
  ASSERT_EQ(std::move(once).Run(), 42);
}

TEST(RepeatingClosure, ConvertsToOnceWithParameters) {
  RepeatingClosure<int(int)> repeating([](int x) { return x * 2; });
  OnceClosure<int(int)> once = std::move(repeating);
  ASSERT_EQ(std::move(once).Run(21), 42);
}

TEST(RepeatingClosure, Reset) {
  RepeatingClosure<void()> closure([] {});
  ASSERT_FALSE(closure.is_null());
  closure.Reset();
  ASSERT_TRUE(closure.is_null());
}

}  // namespace
}  // namespace base
