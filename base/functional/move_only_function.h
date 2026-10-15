#ifndef BASE_FUNCTIONAL_MOVE_ONLY_FUNCTION_H_
#define BASE_FUNCTIONAL_MOVE_ONLY_FUNCTION_H_

#include <cassert>
#include <cstddef>
#include <functional>
#include <new>
#include <type_traits>
#include <utility>

namespace base {

template <typename Signature>
class MoveOnlyFunction;

// A minimal move-only std::function: type-erased ownership of a callable,
// with no copy semantics anywhere (std::move_only_function is the C++23
// standard answer; this is the from-scratch learning implementation).
//
// Two deliberate deviations from std::function:
//  - The callable is invoked through a non-const lvalue, so mutable lambdas
//    are welcome (std::function requires const-invocability).
//  - For signature void(...), any return type of the wrapped callable is
//    silently discarded (std::is_invocable_r with R = void), which makes it
//    a convenient "fire and forget" task type.
//
// Implementation: classic vtable-by-function-pointers type erasure.
// Callables up to three pointers in size (and nothrow-movable) are stored
// inline; larger ones are heap-allocated and the buffer holds the pointer.
template <typename R, typename... Params>
class MoveOnlyFunction<R(Params...)> final {
 public:
  using result_type = R;

  MoveOnlyFunction() noexcept = default;

  // From anything invocable with Params... (excluding ourselves, so the
  // move constructor wins overload resolution for same-type arguments).
  template <typename F>
    requires(!std::is_same_v<std::decay_t<F>, MoveOnlyFunction> &&
             std::is_invocable_r_v<R, F&, Params...>)
  MoveOnlyFunction(F&& f) {
    using D = std::decay_t<F>;
    if constexpr (kStoreInline<D>) {
      ::new (static_cast<void*>(storage_.inline_)) D(std::forward<F>(f));
      ops_ = &kOps<D, true>;
    } else {
      storage_.heap = new D(std::forward<F>(f));
      ops_ = &kOps<D, false>;
    }
  }

  MoveOnlyFunction(MoveOnlyFunction const&) = delete;
  MoveOnlyFunction& operator=(MoveOnlyFunction const&) = delete;

  MoveOnlyFunction(MoveOnlyFunction&& other) noexcept : ops_(other.ops_) {
    if (ops_ != nullptr) {
      ops_->relocate(&storage_, &other.storage_);
      other.ops_ = nullptr;
    }
  }

  MoveOnlyFunction& operator=(MoveOnlyFunction&& other) noexcept {
    if (this != &other) {
      Reset();
      ops_ = other.ops_;
      if (ops_ != nullptr) {
        ops_->relocate(&storage_, &other.storage_);
        other.ops_ = nullptr;
      }
    }
    return *this;
  }

  ~MoveOnlyFunction() { Reset(); }

  // Precondition: not empty (asserted; std::function throws
  // std::bad_function_call here, we treat it as a logic error).
  R operator()(Params... params) {
    assert(ops_ != nullptr && "invoking an empty MoveOnlyFunction");
    return ops_->invoke(&storage_, std::forward<Params>(params)...);
  }

  explicit operator bool() const noexcept { return ops_ != nullptr; }

  void Reset() noexcept {
    if (ops_ != nullptr) {
      ops_->destroy(&storage_);
      ops_ = nullptr;
    }
  }

 private:
  static constexpr std::size_t kInlineCapacity = 3 * sizeof(void*);
  union Storage {
    void* heap;
    alignas(std::max_align_t) std::byte inline_[kInlineCapacity];
  };

  // One statically instantiated ops table per (callable type, storage mode)
  // pair; empty is represented by a null ops_ pointer, so no table entry is
  // ever spent on it.
  struct Ops {
    R (*invoke)(Storage*, Params&&...);
    // Move-constructs the callable into dst and vacates src (for the heap
    // mode, "move" is just the pointer copy).
    void (*relocate)(Storage* dst, Storage* src) noexcept;
    void (*destroy)(Storage*) noexcept;
  };

  template <typename D>
  static constexpr bool kStoreInline =
      sizeof(D) <= kInlineCapacity &&
      alignof(D) <= alignof(std::max_align_t) &&
      std::is_nothrow_move_constructible_v<D>;

  template <typename D, bool kInline>
  static D& At(Storage* storage) noexcept {
    if constexpr (kInline) {
      return *std::launder(reinterpret_cast<D*>(storage->inline_));
    }
    return *static_cast<D*>(storage->heap);
  }

  template <typename D, bool kInline>
  static inline constexpr Ops kOps{
      [](Storage* storage, Params&&... params) -> R {
        if constexpr (std::is_void_v<R>) {
          // The wrapped callable may still return something; the value is
          // deliberately discarded for void signatures.
          std::invoke(At<D, kInline>(storage),
                      std::forward<Params>(params)...);
        } else {
          return std::invoke(At<D, kInline>(storage),
                             std::forward<Params>(params)...);
        }
      },
      [](Storage* dst, Storage* src) noexcept {
        if constexpr (kInline) {
          ::new (static_cast<void*>(dst->inline_))
              D(std::move(At<D, true>(src)));
          At<D, true>(src).~D();
        } else {
          dst->heap = src->heap;
        }
      },
      [](Storage* storage) noexcept {
        if constexpr (kInline) {
          At<D, true>(storage).~D();
        } else {
          delete static_cast<D*>(storage->heap);
        }
      }};

  Ops const* ops_ = nullptr;
  Storage storage_;
};

}  // namespace base

#endif  // BASE_FUNCTIONAL_MOVE_ONLY_FUNCTION_H_
