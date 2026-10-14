/**
 * @file smart_ptr.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_SMART_PTR_H_
#define BASE_SMART_PTR_H_

#include <cstddef>
#include "base/memory/internal/shared_ptr.h"

namespace base {

template <typename Tp>
class SharedPtr;
template <typename Type>
class WeakPtr;
template <typename Type, typename Deleter>
class UinquePtr;

/**
 * @brief Shared pointer.
 *
 * @tparam Tp
 */
template <typename Tp>
class SharedPtr final : private internal::SharedPtrBase<Tp> {
 public:
#if __cpluscplus < 201703L
  using element_type = Tp;
#else
  using element_type = std::remove_extent<T>;
  using weak_type = WeakPtr<T>;
#endif
  /**
   * @brief Constructor 1~2. Constructs a shared_ptr with no managed object,
   * i.e. empty shared_ptr.
   *
   */
  constexpr SharedPtr() noexcept : internal::SharedPtrBase<Tp>() {}  // 1
  constexpr SharedPtr(std::nullptr_t)
      : internal::SharedPtrBase<Tp>(std::nullptr_t{}) {}  // 2

  /**
   * @brief Constructor 3~7. Constructs a shared_ptr with ptr as the pointer to
   * the managed object.
   *
   * @tparam Up
   * @param p
   */
  template <typename Up>
  explicit SharedPtr(Up* p);  // 3
  template <typename Up, typename Deleter>
  SharedPtr(Up* p, Deleter d);  // 4
  template <typename Deleter>
  SharedPtr(std::nullptr_t ptr, Deleter d);  // 5
  template <typename Up, typename Deleter, typename Allocator>
  SharedPtr(Up* ptr, Deleter d, Allocator alloc);  // 6
  template <typename Deleter, typename Allocator>
  SharedPtr(std::nullptr_t ptr, Deleter d, Allocator alloc);  // 7

  /**
   * @brief The aliasing constructor: constructs a shared_ptr which shares
   * ownership information with the initial value of r, but holds an unrelated
   * and unmanaged pointer ptr. If this shared_ptr is the last of the group to
   * go out of scope, it will call the stored deleter for the object originally
   * managed by r. However, calling get() on this shared_ptr will always return
   * a copy of ptr. It is the responsibility of the programmer to make sure that
   * this ptr remains valid as long as this shared_ptr exists, such as in the
   * typical use cases where ptr is a member of the object managed by r or is an
   * alias (e.g., downcast) of r.get() For the second overload taking an rvalue,
   * r is empty and r.get() == nullptr after the call.(since C++20)
   *
   */
  template <typename Up>
  SharedPtr(SharedPtr<Up> const& r, element_type* ptr) noexcept;  // 8

  /**
   * @brief Constructs a shared_ptr which shares ownership of the object managed
   * by r. If r manages no object, *this manages no object either. The template
   * overload doesn't participate in overload resolution if Y* is not implicitly
   * convertible to(until C++17)compatible with(since C++17) T*.
   *
   * @tparam Up
   * @param r
   */
  template <typename Up>
  SharedPtr(SharedPtr<Up> const& r) noexcept;  // 9

  /**
   * @brief  Move-constructs a shared_ptr from r. After the construction, *this
   * contains a copy of the previous state of r, r is empty and its stored
   * pointer is null. The template overload doesn't participate in overload
   * resolution if Y* is not implicitly convertible to(until C++17)compatible
   * with(since C++17) T*.
   * @tparam Up
   * @param r
   */
  template <typename Up>
  SharedPtr(SharedPtr<Up>&& r) noexcept;  // 10

  template <typename Up>
  explicit SharedPtr(base::WeakPtr<Up> const& r) noexcept;  // 11

  template <typename Up, typename Deleter>
  SharedPtr(base::UinquePtr<Up, Deleter>&& r);  // 12
};

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {}

template <typename T, typename... Args>
SharedPtr<T> MakeUnique(Args&&... args) {}

}  // namespace base
#endif