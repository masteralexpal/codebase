/**
 * @file shared_ptr.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <atomic>
#include <cstddef>

#ifndef BASE_MEMORY_INTERNAL_SHARED_PTR_H_
#define BASE_MEMORY_INTERNAL_SHARED_PTR_H_

namespace base {

namespace internal {

template <typename Tp>
class SharedPtrBase {
 public:
  constexpr SharedPtrBase() noexcept = default;
  constexpr SharedPtrBase(std::nullptr_t) noexcept = default;

 private:
  using reference_counting_type = std::atomic<int>;
  struct __deleter_base {
    virtual void __delete(void* p) = 0;
    virtual ~__deleter_base() noexcept = default;
  };

  template <typename Deleter>
  struct __deleter : public __deleter_base {
    using element_type = Tp;
    void __delete(void* p) { deleter_(static_cast<element_type*>(p)); }
    Deleter deleter_;
  };

  reference_counting_type shared_count_{0};
  reference_counting_type weak_count_{0};
  __deleter_base* deleter_{nullptr};
};

template <typename Tp>
class ReferenceCounter {
 public:
 private:
  using reference_counting_type = std::atomic<int>;

  reference_counting_type shared_reference_count_{0};
  reference_counting_type weak_reference_count_{0};
  Tp* object{nullptr};
};
}  // namespace internal
}  // namespace base

#endif  // BASE_MEMORY_INTERNAL_SHARED_PTR_H_