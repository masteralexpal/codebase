#include "base/functional/move_only_function.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace base {
namespace {

using impl::MoveOnlyFunction;

int Add(int a, int b) {
  return a + b;
}

// Counts live instances to observe construction/destruction through the
// type erasure.
struct LifecycleCounter {
  static inline int live = 0;

  LifecycleCounter() { ++live; }
  LifecycleCounter(LifecycleCounter const&) { ++live; }
  LifecycleCounter(LifecycleCounter&&) noexcept { ++live; }
  ~LifecycleCounter() { --live; }

  void operator()() {}
};

// Same counting, but too big for the inline buffer: exercises the heap
// path's lifecycle.
struct BigLifecycleCounter {
  static inline int live = 0;

  BigLifecycleCounter() { ++live; }
  BigLifecycleCounter(BigLifecycleCounter const&) { ++live; }
  BigLifecycleCounter(BigLifecycleCounter&&) noexcept { ++live; }
  ~BigLifecycleCounter() { --live; }

  std::byte payload[128];
  void operator()() {}
};

TEST(MoveOnlyFunction, EmptyByDefault) {
  MoveOnlyFunction<void()> f;
  ASSERT_FALSE(static_cast<bool>(f));
  // Resetting an empty one is a no-op.
  f.Reset();
  ASSERT_FALSE(static_cast<bool>(f));
}

TEST(MoveOnlyFunction, DefaultConstructionIsNothrow) {
  // The default constructor is declared constexpr (mirroring
  // std::move_only_function) so that OnceClosure's constexpr declaration
  // stays well-formed, but a constexpr *object* is impossible: the
  // destructor is not constexpr, so the type is not a literal type.
  static_assert(
      std::is_nothrow_default_constructible_v<MoveOnlyFunction<void()>>);
}

TEST(MoveOnlyFunction, NullFunctionPointerYieldsEmpty) {
  int (*null_fn)(int, int) = nullptr;
  MoveOnlyFunction<int(int, int)> f(null_fn);
  ASSERT_FALSE(static_cast<bool>(f));
}

TEST(MoveOnlyFunction, NotCopyable) {
  static_assert(!std::is_copy_constructible_v<MoveOnlyFunction<void()>>);
  static_assert(!std::is_copy_assignable_v<MoveOnlyFunction<void()>>);
}

TEST(MoveOnlyFunction, ResultTypeIsExposed) {
  static_assert(
      std::is_same_v<MoveOnlyFunction<int(int)>::result_type, int>);
}

TEST(MoveOnlyFunction, InvokesFreeFunction) {
  MoveOnlyFunction<int(int, int)> f(&Add);
  ASSERT_EQ(f(1, 2), 3);
}

TEST(MoveOnlyFunction, InvokesStdFunctionTarget) {
  std::function<int(int)> doubler([](int x) { return x * 2; });
  MoveOnlyFunction<int(int)> f(std::move(doubler));
  ASSERT_EQ(f(21), 42);
}

TEST(MoveOnlyFunction, InvokesLambdaWithParameters) {
  MoveOnlyFunction<std::string(std::string const&)> f(
      [](std::string const& s) { return s + s; });
  ASSERT_EQ(f("ab"), "abab");
}

TEST(MoveOnlyFunction, ForwardsMoveOnlyParameterAtCallSite) {
  MoveOnlyFunction<int(std::unique_ptr<int>)> f(
      [](std::unique_ptr<int> p) { return *p; });
  ASSERT_EQ(f(std::make_unique<int>(42)), 42);
}

TEST(MoveOnlyFunction, AcceptsMutableLambda) {
  // std::function would reject this: the call goes through a non-const
  // reference here.
  MoveOnlyFunction<int()> f([count = 0]() mutable { return ++count; });
  ASSERT_EQ(f(), 1);
  ASSERT_EQ(f(), 2);
}

TEST(MoveOnlyFunction, AcceptsMoveOnlyCallable) {
  auto p = std::make_unique<int>(42);
  MoveOnlyFunction<int()> f([ptr = std::move(p)] { return *ptr; });
  ASSERT_EQ(f(), 42);
}

TEST(MoveOnlyFunction, DiscardsReturnValueForVoidSignature) {
  bool called = false;
  MoveOnlyFunction<void()> f([&called]() -> int {
    called = true;
    return 42;
  });
  f();
  ASSERT_TRUE(called);
}

TEST(MoveOnlyFunction, MoveConstructorVacatesSource) {
  MoveOnlyFunction<int()> first([] { return 7; });
  MoveOnlyFunction<int()> second(std::move(first));
  ASSERT_FALSE(static_cast<bool>(first));
  ASSERT_TRUE(static_cast<bool>(second));
  ASSERT_EQ(second(), 7);
}

TEST(MoveOnlyFunction, MoveConstructorFromEmptyYieldsEmpty) {
  MoveOnlyFunction<void()> first;
  MoveOnlyFunction<void()> second(std::move(first));
  ASSERT_FALSE(static_cast<bool>(second));
}

TEST(MoveOnlyFunction, MoveAssignmentDestroysPreviousContent) {
  LifecycleCounter::live = 0;
  {
    MoveOnlyFunction<void()> doomed(LifecycleCounter{});
    ASSERT_EQ(LifecycleCounter::live, 1);
    doomed = MoveOnlyFunction<void()>([] {});
    // The previously held callable was destroyed by the assignment.
    ASSERT_EQ(LifecycleCounter::live, 0);
  }
}

TEST(MoveOnlyFunction, MoveAssignmentFromEmptyClearsTarget) {
  LifecycleCounter::live = 0;
  MoveOnlyFunction<void()> f(LifecycleCounter{});
  MoveOnlyFunction<void()> empty;
  f = std::move(empty);
  ASSERT_FALSE(static_cast<bool>(f));
  ASSERT_EQ(LifecycleCounter::live, 0);
}

TEST(MoveOnlyFunction, SelfMoveAssignmentKeepsContent) {
  int count = 0;
  MoveOnlyFunction<void()> f([&count] { ++count; });
  MoveOnlyFunction<void()>& alias = f;
  f = std::move(alias);
  ASSERT_TRUE(static_cast<bool>(f));
  f();
  ASSERT_EQ(count, 1);
}

TEST(MoveOnlyFunction, InlineCallableLifecycleIsExact) {
  LifecycleCounter::live = 0;
  {
    MoveOnlyFunction<void()> f(LifecycleCounter{});
    ASSERT_EQ(LifecycleCounter::live, 1);
    MoveOnlyFunction<void()> g(std::move(f));
    // Relocation: move-construct into g, destroy in f — still one instance.
    ASSERT_EQ(LifecycleCounter::live, 1);
  }
  ASSERT_EQ(LifecycleCounter::live, 0);
}

TEST(MoveOnlyFunction, HeapCallableLifecycleIsExact) {
  BigLifecycleCounter::live = 0;
  {
    MoveOnlyFunction<void()> f(BigLifecycleCounter{});
    ASSERT_EQ(BigLifecycleCounter::live, 1);
    MoveOnlyFunction<void()> g(std::move(f));
    // The heap pointer changes hands; nothing is constructed or destroyed.
    ASSERT_EQ(BigLifecycleCounter::live, 1);
    g.Reset();
    ASSERT_EQ(BigLifecycleCounter::live, 0);
  }
  ASSERT_EQ(BigLifecycleCounter::live, 0);
}

TEST(MoveOnlyFunction, OverAlignedCallableWorks) {
  // Alignment is padded into sizeof, so an over-aligned type is always
  // oversized too; either check routes it to the heap.
  struct alignas(64) OverAligned {
    int operator()() { return 42; }
  };
  static_assert(alignof(OverAligned) > alignof(std::max_align_t));
  MoveOnlyFunction<int()> f(OverAligned{});
  ASSERT_EQ(f(), 42);
}

TEST(MoveOnlyFunction, ThrowingMoveCallableUsesHeapPath) {
  // Small enough for the inline buffer, but a throwing move constructor
  // disqualifies inline storage (relocation must stay noexcept).
  struct ThrowingMove {
    ThrowingMove() = default;
    ThrowingMove(ThrowingMove&&) noexcept(false) {}
    int operator()() { return 42; }
  };
  static_assert(sizeof(ThrowingMove) <= 3 * sizeof(void*));
  MoveOnlyFunction<int()> f(ThrowingMove{});
  MoveOnlyFunction<int()> g(std::move(f));
  ASSERT_EQ(g(), 42);
}

TEST(MoveOnlyFunction, RebindAfterReset) {
  MoveOnlyFunction<int()> f([] { return 1; });
  ASSERT_EQ(f(), 1);
  f.Reset();
  ASSERT_FALSE(static_cast<bool>(f));
  f = MoveOnlyFunction<int()>([] { return 2; });
  ASSERT_EQ(f(), 2);
}

TEST(MoveOnlyFunction, LargeCallableGoesThroughHeap) {
  // Bigger than the inline buffer, so this takes the heap path.
  struct Big {
    std::byte payload[128];
    int value = 42;
    int operator()() { return value; }
  };
  static_assert(sizeof(Big) > 3 * sizeof(void*));
  MoveOnlyFunction<int()> f(Big{});
  ASSERT_EQ(f(), 42);
  MoveOnlyFunction<int()> g(std::move(f));
  ASSERT_EQ(g(), 42);
}

TEST(MoveOnlyFunction, ResetDestroysContent) {
  LifecycleCounter::live = 0;
  MoveOnlyFunction<void()> f(LifecycleCounter{});
  ASSERT_EQ(LifecycleCounter::live, 1);
  f.Reset();
  ASSERT_EQ(LifecycleCounter::live, 0);
  ASSERT_FALSE(static_cast<bool>(f));
}

}  // namespace
}  // namespace base
