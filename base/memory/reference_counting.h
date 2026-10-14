/**
 * @file reference_counting.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_MEMORY_REFERENCE_COUNTING_H_
#define BASE_MEMORY_REFERENCE_COUNTING_H_

#include <atomic>
#include <memory>
#include <utility>

namespace base {
class ReferenceCountingBase {
 public:
  ReferenceCountingBase() noexcept;

  explicit ReferenceCountingBase(int initial_count) noexcept;

  ~ReferenceCountingBase() noexcept = default;

  /**
   * @brief Add the reference count by 1.
   *
   */
  void Add() const noexcept;

  /**
   * @brief Subtract the reference count by 1.
   *
   */
  bool Sub() const noexcept;

  /**
   * @brief Get the reference count.
   *
   * @return int
   */
  int Count() const noexcept;

  /**
   * @brief  Check if the current oject has multiple referrer.
   *
   * @return true
   * @return false
   */
  bool Shared() const noexcept;

  /**
   * @brief Check if the current oject has only one referrer.
   *
   * @return true
   * @return false
   */
  bool Unique() const noexcept;

 private:
  int mutable count_;
};

template <typename Type>
struct LifetimeTraits {
  template <typename... Args>
  static Type* New(Args&&... args) {
    return new Type(std::forward<Args>(args)...);
  }

  static void Delete(Type const* pointer) { delete pointer; }
};

/**
 * @brief
 *
 * @tparam Type
 * @tparam Traits
 */
template <typename Type, typename Traits = LifetimeTraits<Type>>
class ReferenceCounting : public ReferenceCountingBase {
 public:
  using value_type = Type;
  using traits_type = Traits;

  ReferenceCounting() noexcept : ReferenceCountingBase() {}

  explicit ReferenceCounting(int initial_value) noexcept
      : ReferenceCountingBase(initial_value) {}

  ReferenceCounting(ReferenceCounting const& other) = delete;
  ReferenceCounting& operator=(ReferenceCounting const& other) = delete;

  void Add() const noexcept { ReferenceCountingBase::Add(); }

  // Throwing exceptions from destructor is dangerous, therefore Sub() marked
  // "noexcept"
  void Sub() const noexcept {
    if (ReferenceCountingBase::Sub()) {
      Traits::Delete(static_cast<value_type const*>(this));
    }
  }
};
}  // namespace base
#endif  //  BASE_MEMORY_REFERENCE_COUNTING_H_