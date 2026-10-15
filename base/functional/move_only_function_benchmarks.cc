// Benchmark: base::impl::MoveOnlyFunction against the established type
// erasures — std::function, absl::AnyInvocable, and (where the standard
// library provides it) std::move_only_function. A raw function pointer is
// timed as the floor.
//
// Three dimensions:
//   Invoke          — steady-state call overhead through the erasure.
//   ConstructSmall  — construct + destroy a callable that fits the inline
//                     buffer (SBO) of every contender.
//   ConstructBig    — same, but the callable forces the heap path.
//   MoveSmall/Big   — relocation: buffer shuffle vs pointer hand-off.

#include <benchmark/benchmark.h>

#include <cstddef>
#include <functional>

#include "absl/functional/any_invocable.h"
#include "base/functional/move_only_function.h"

namespace base {
namespace {

using impl::MoveOnlyFunction;

// The payload that sends every contender to the heap (all three have an
// inline buffer of 2-3 pointers).
struct Big {
  explicit Big(int v) : value(v) {}

  std::byte payload[128];
  int value;
  int operator()() { return value; }
};

// int-returning callables: the result feeds DoNotOptimize so the call
// itself is never optimized away.

template <typename F>
void BM_Invoke(benchmark::State& state) {
  int seed = 42;
  // DoNotOptimize forces the capture (and with it the erasure object) to
  // materialize; otherwise the whole call chain constant-folds.
  benchmark::DoNotOptimize(seed);
  F f([x = seed] { return x; });
  benchmark::DoNotOptimize(f);
  int sink = 0;
  for (auto _ : state) {
    sink += f();
  }
  benchmark::DoNotOptimize(sink);
}

template <typename F>
void BM_ConstructSmall(benchmark::State& state) {
  int seed = static_cast<int>(state.iterations());
  benchmark::DoNotOptimize(seed);
  for (auto _ : state) {
    // The capture depends on the loop state so construction cannot be
    // hoisted or constant-folded.
    F f([x = seed] { return x; });
    benchmark::DoNotOptimize(f);
    ++seed;
  }
}

template <typename F>
void BM_ConstructBig(benchmark::State& state) {
  int seed = static_cast<int>(state.iterations());
  benchmark::DoNotOptimize(seed);
  for (auto _ : state) {
    F f([big = Big{seed}] { return big.value; });
    benchmark::DoNotOptimize(f);
    ++seed;
  }
}

template <typename F>
void BM_MoveSmall(benchmark::State& state) {
  F a([x = 42] { return x; });
  F b;
  for (auto _ : state) {
    b = std::move(a);
    benchmark::DoNotOptimize(b);
    a = std::move(b);
    benchmark::DoNotOptimize(a);
  }
}

template <typename F>
void BM_MoveBig(benchmark::State& state) {
  F a([big = Big{42}] { return big.value; });
  F b;
  for (auto _ : state) {
    b = std::move(a);
    benchmark::DoNotOptimize(b);
    a = std::move(b);
    benchmark::DoNotOptimize(a);
  }
}

// The floor: an indirect call through a plain function pointer.
void BM_InvokeRawFunctionPointer(benchmark::State& state) {
  int (*f)(int) = [](int x) { return x; };
  // Without this the compiler devirtualizes the known pointer and folds
  // the whole loop.
  benchmark::DoNotOptimize(f);
  int sink = 0;
  for (auto _ : state) {
    sink += f(1);
  }
  benchmark::DoNotOptimize(sink);
}

BENCHMARK(BM_InvokeRawFunctionPointer);
BENCHMARK(BM_Invoke<std::function<int()>>);
BENCHMARK(BM_Invoke<absl::AnyInvocable<int()>>);
BENCHMARK(BM_Invoke<MoveOnlyFunction<int()>>);
#ifdef __cpp_lib_move_only_function
BENCHMARK(BM_Invoke<std::move_only_function<int()>>);
#endif

BENCHMARK(BM_ConstructSmall<std::function<int()>>);
BENCHMARK(BM_ConstructSmall<absl::AnyInvocable<int()>>);
BENCHMARK(BM_ConstructSmall<MoveOnlyFunction<int()>>);
#ifdef __cpp_lib_move_only_function
BENCHMARK(BM_ConstructSmall<std::move_only_function<int()>>);
#endif

BENCHMARK(BM_ConstructBig<std::function<int()>>);
BENCHMARK(BM_ConstructBig<absl::AnyInvocable<int()>>);
BENCHMARK(BM_ConstructBig<MoveOnlyFunction<int()>>);
#ifdef __cpp_lib_move_only_function
BENCHMARK(BM_ConstructBig<std::move_only_function<int()>>);
#endif

BENCHMARK(BM_MoveSmall<std::function<int()>>);
BENCHMARK(BM_MoveSmall<absl::AnyInvocable<int()>>);
BENCHMARK(BM_MoveSmall<MoveOnlyFunction<int()>>);
#ifdef __cpp_lib_move_only_function
BENCHMARK(BM_MoveSmall<std::move_only_function<int()>>);
#endif

BENCHMARK(BM_MoveBig<std::function<int()>>);
BENCHMARK(BM_MoveBig<absl::AnyInvocable<int()>>);
BENCHMARK(BM_MoveBig<MoveOnlyFunction<int()>>);
#ifdef __cpp_lib_move_only_function
BENCHMARK(BM_MoveBig<std::move_only_function<int()>>);
#endif

}  // namespace
}  // namespace base
