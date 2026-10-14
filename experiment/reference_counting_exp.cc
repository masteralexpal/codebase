/**
 * @file reference_counting_exp.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <utility>

namespace base {
template <typename Type>
class Reference {
 public:
  using value_type = Type;
  using pointer = value_type*;
  using const_pointer = value_type const*;
  using reference = value_type&;
  using const_reference = value_type const&;

  /**
   * @brief Default constructor of Reference.
   *
   */
  Reference() noexcept = default;

  /**
   * @brief Construct a new Reference object
   *
   * @param ptr
   */
  explicit Reference(pointer ptr) : pointer_(ptr) {
    if (pointer_) {
      pointer_->Increase();
    }
  }

  explicit Reference(const_pointer const_ptr) : pointer_(const_ptr) {
    if (pointer_) {
      pointer_->Increase();
    }
  }

  template <typename... Args>
  Reference(Args&&... args) : pointer_(new Type(std::forward<Args>(args)...)) {}

  explicit Reference(std::nullptr_t) noexcept {}

  bool operator==(std::nullptr_t) noexcept { return pointer_ == nullptr; }

  /**
   * @brief Copy constructor
   *
   * @param other
   */
  Reference(Reference const& other) : pointer_(other.pointer_) {
    if (pointer_) {
      pointer_->Increase();
    }
  }
  Reference& operator=(Reference const& other) {
    if (this != &other) {
      if (pointer_) {
        pointer_->Decrease();
      }
      pointer_ = other.pointer_;
      if (pointer_) {
        pointer_->Increase();
      }
    }
    return *this;
  }
  Reference(Reference&& other) : pointer_(other.pointer_) {
    other.pointer_ = nullptr;
  }
  Reference& operator=(Reference&& other) {
    pointer_ = other.pointer_;
    other.pointer_->Decrease();
    other.pointer_ = nullptr;
  }
  ~Reference() {
    if (pointer_ && pointer_->Decrease()) {
      std::cout << "Bye-bye~" << std::endl;
    }
  }

  pointer operator->() noexcept { return pointer_; }

  const_pointer operator->() const noexcept { return pointer_; }

  reference operator*() noexcept { return *pointer_; }

  const_reference operator*() const noexcept { return *pointer_; }

  bool operator==(std::nullptr_t) const noexcept { return pointer_ == nullptr; }

  template <typename U>
  bool operator==(Reference<U> ptr) {
    return pointer_ == ptr.pointer_;
  }

 private:
  pointer pointer_ = nullptr;
};

template <typename Type>
class RefCnt {
 public:
  RefCnt() noexcept : cnt_(1) {}

  explicit RefCnt(std::int32_t initial_value) noexcept : cnt_(initial_value) {}

  void Increase() const noexcept { ++cnt_; }

  bool Decrease() const noexcept { return --cnt_ == 0; }

  std::int32_t Count() const noexcept { return cnt_; }

  ~RefCnt() { std::cout << "Bye-bye~\n"; }

 private:
  template <typename U>
  friend class Pointer;
  std::int32_t mutable cnt_;
};

class A : public RefCnt<A> {
 public:
  A() noexcept {}

 private:
  ~A();
};

// Reference: https://en.cppreference.com/w/cpp/types/enable_if

/* WRONG */

// struct T
// {
//     enum { int_t, float_t } type;

//     template<typename Integer,
//              typename = std::enable_if_t<std::is_integral<Integer>::value>>
//     T(Integer) : type(int_t) {}

//     template<typename Floating,
//              typename =
//              std::enable_if_t<std::is_floating_point<Floating>::value>>
//     T(Floating) : type(float_t) {} // error: treated as redefinition
// };

/* RIGHT */

struct T {
  enum { int_t, float_t } type;

  template <typename Integer,
            std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
  T(Integer) : type(int_t) {}

  template <
      typename Floating,
      std::enable_if_t<std::is_floating_point<Floating>::value, bool> = true>
  T(Floating) : type(float_t) {}  // OK
};

template <typename Type,
          typename std::enable_if<std::is_base_of<RefCnt<Type>, Type>::value,
                                  bool>::type _ = true>
void TestRefCntEnableIf(Type const& value) {
  return;
}

template <
    typename Type,
    typename std::enable_if<std::is_integral<Type>::value, bool>::type _ = true>
void TestRefCntEnableIf(Type const& value) {
  return;
}

template <typename Type,
          typename... Args,
          typename std::enable_if<std::is_base_of<RefCnt<Type>, Type>::value,
                                  bool>::type _ = true>
auto MakeRefCnt(Args&&... args) -> Reference<Type> {
  return Reference<Type>(new Type(std::forward<Args>(args)...));
}

template <typename Type,
          typename... Args,
          typename std::enable_if<std::is_base_of<RefCnt<Type>, Type>::value,
                                  bool>::type _ = true>
auto MakeReference(Args&&... args) -> Reference<Type> {
  return Reference<Type>(std::forward<Args>(args)...);
}

template <typename Type,
          typename std::enable_if<std::is_base_of<RefCnt<Type>, Type>::value,
                                  bool>::type = true>
auto WrapRefCnt(Type const& type) -> Reference<Type> {
  return Reference<Type>{&type};
}

class B : public RefCnt<B> {
 public:
  B() = default;
  ~B() = default;
};
}  // namespace base

int main(int argc, char** argv) {
  base::Reference<base::A> ptr = base::MakeRefCnt<base::A>();
  assert(ptr->Count() == 1);
  ptr->Increase();
  ptr->Decrease();
  // Oops
  std::cout << (*ptr).Count() << std::endl;
  base::A* a = new base::A();
  base::B b;
  base::TestRefCntEnableIf(b);
  int c = 1;
  base::TestRefCntEnableIf(c);
}