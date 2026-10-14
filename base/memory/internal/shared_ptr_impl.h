/**
 * @file shared_ptr_impl.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <setjmp.h>
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace base {
namespace internal {}  // namespace internal

template <typename Type>
class SharedPtr {
 public:
  using element_type = std::remove_extent_t<Type>;
  using weak_type = std::weak_ptr<Type>;
  /**
   * @brief Constructs a shared_ptr with no managed object, i.e. empty
   * shared_ptr.
   *
   */

  constexpr explicit SharedPtr() {
    // C.1
  }
  constexpr explicit SharedPtr(std::nullptr_t) {
    // C.2
  }

  /**
   * @brief Constructs a shared_ptr with ptr as the pointer to the managed
   * object.
   *
   * @tparam U - U* must be convertible to Type*.
   * @param ptr
   */
  template <
      typename U,
      typename std::enable_if<std::is_convertible<U*, element_type*>::value,
                              bool>::type = true>
  explicit SharedPtr(U* ptr) : storage_(new Storage(ptr, 1, 0)) {
    // C.3
  }

  template <
      typename U,
      typename Deleter,
      typename std::enable_if<std::is_convertible<U*, element_type*>::value,
                              bool>::type = true,
      typename std::enable_if<std::is_copy_constructible<Deleter>::value,
                              bool>::type = true>
  SharedPtr(U* ptr, Deleter deleter)
      : storage_(new Storage(ptr, 1, 0)),
        deleter_(new DeleterImpl<Deleter>(std::move(deleter))) {
    // C.4
  }

  template <class Deleter,
            typename std::enable_if<std::is_copy_constructible<Deleter>::value,
                                    bool>::type = true>
  SharedPtr(std::nullptr_t, Deleter d)
      : deleter_(new DeleterImpl<Deleter>(std::move(d))) {
    // C.5
  }

  template <
      class U,
      class Deleter,
      class Alloc,
      typename std::enable_if<std::is_convertible<U*, element_type*>::value,
                              bool>::type = true,
      typename std::enable_if<std::is_copy_constructible<Deleter>::value,
                              bool>::type = true>
  SharedPtr(U* ptr, Deleter deleter, Alloc alloc)
      : storage_(new Storage(ptr, 1, 0), deleter_(std::move(deleter))) {
    // C.6
    // TODO
    std::ignore = alloc;
  }

  template <class Deleter,
            class Alloc,
            typename std::enable_if<std::is_copy_constructible<Deleter>::value,
                                    bool>::type = true>
  SharedPtr(std::nullptr_t, Deleter d, Alloc alloc)
      : deleter_(new DeleterImpl<Deleter>(std::move(d))) {
    // C.7
    // TODO
    std::ignore = alloc;
  }

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
   * @tparam U
   * @param r
   * @param ptr
   */
  template <typename U,
            typename std::enable_if<std::is_convertible<U*, Type*>::value,
                                    bool>::type = true>
  SharedPtr(SharedPtr<U> const& r, element_type* ptr) noexcept {
    // C.8
    // TODO
    std::ignore = r;
    std::ignore = ptr;
  }

  /**
   * @brief Constructs a shared_ptr which shares ownership of the object managed
   * by r. If r manages no object, *this manages no object either. The template
   * overload doesn't participate in overload resolution if Y* is not implicitly
   * convertible to(until C++17)compatible with(since C++17) T*.
   *
   * @tparam U
   * @param r
   * @param ptr
   */
  template <class U>
  SharedPtr(SharedPtr<U>&& r, element_type* ptr) noexcept {
    // C.8
    // TODO
    std::ignore = r;
    std::ignore = ptr;
  }

  SharedPtr(SharedPtr const& r) noexcept
      : storage_(r.storage_), deleter_(r.deleter_) {
    // C.9
    storage_->AddSharedRef();
  }

  template <class U,
            typename std::enable_if<std::is_convertible<U*, Type*>::value,
                                    bool>::type = true>
  SharedPtr(SharedPtr<U> const& r) noexcept
      : storage_(r.storage_), deleter_(r.deleter_) {
    // C.9
    storage_->AddSharedRef();
  }

  SharedPtr(SharedPtr&& r) noexcept {
    // C.10
    std::swap(storage_, r.storage_);
    std::swap(deleter_, r.deleter_);
  }

  template <class U,
            typename std::enable_if<std::is_convertible<U*, Type*>::value,
                                    bool>::type = true>
  SharedPtr(SharedPtr<U>&& r) noexcept {
    // C.10
    std::swap(storage_, r.storage_);
    std::swap(deleter_, r.deleter_);
  }

  /**
   * @brief Constructs a shared_ptr which shares ownership of the object
   * managed by r. Y* must be implicitly convertible to T*.(until C++17)This
   * overload participates in overload resolution only if Y* is compatible
   * with T*.(since C++17) Note that r.lock() may be used for the same
   * purpose: the difference is that this constructor throws an exception if
   * the argument is empty, while std::weak_ptr<T>::lock() constructs an empty
   * std::shared_ptr in that case
   *
   * @tparam U
   * @param r
   */
  template <typename U,
            typename std::enable_if<std::is_convertible<U*, Type*>::value,
                                    bool>::type = true>
  explicit SharedPtr(std::weak_ptr<U> const& r) {
    // C.11
    // TODO
    std::ignore = r;
  }

  /**
   * @brief Constructs a shared_ptr which manages the object currently managed
   * by r. The deleter associated with r is stored for future deletion of the
   * managed object. r manages no object after the call.
   *
   * @tparam U
   * @tparam Deleter
   * @param r
   */
  template <typename U,
            typename Deleter,
            typename std::enable_if<std::is_convertible<U*, Type*>::value,
                                    bool>::type = true,
            typename std::enable_if<std::is_copy_constructible<Deleter>::value,
                                    bool>::type = true>
  SharedPtr(std::unique_ptr<U, Deleter>&& r) {
    // C.13
    // TODO
    std::ignore = r;
  }

  ~SharedPtr() {}

 private:
  class Storage {
   public:
    Storage() noexcept = default;

    Storage(element_type* pointer, int shared_ref, int weak_ref)
        : pointer_(pointer), shared_ref_(shared_ref), weak_ref_(weak_ref) {}

    inline void AddWeakRef() noexcept { ++weak_ref_; }
    inline void SubWeakRef() noexcept { --weak_ref_; }
    inline void AddSharedRef() noexcept { ++shared_ref_; }
    inline void SubSharedRef() noexcept { --shared_ref_; }

    inline int GetWeakRef() const noexcept { return weak_ref_; }
    inline int GetSharedRef() const noexcept { return shared_ref_; }

    inline void Set(element_type* pointer) noexcept { pointer_ = pointer; }
    inline element_type* Get() const noexcept { return pointer_; }

   private:
    element_type* pointer_{nullptr};
    int shared_ref_{0};
    int weak_ref_{0};
  };

  struct Deleter {
    virtual void Delete(element_type* pointer) = 0;
  };

  template <typename _Deleter>
  class DeleterImpl : Deleter {
    DeleterImpl(_Deleter&& d) : deleter_(d) {}
    void Delete(element_type* pointer) override { deleter_(pointer); }
    _Deleter deleter_;
  };

  Storage* storage_{nullptr};
  Deleter* deleter_{nullptr};
};
}  // namespace base