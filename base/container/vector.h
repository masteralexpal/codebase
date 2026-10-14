/**
 * @file vector.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_ALGORITHM_VECTOR_H_
#define BASE_ALGORITHM_VECTOR_H_

#include <cstddef>
#include <limits>
#include <memory>
#include <tuple>

#include "base/allocator/allocator.h"
#include "base/container/internal/iterator.h"
#include "base/memory/lifetime.h"

namespace base {

template <typename T, typename Allocator>
class VectorBase {
 public:
  VectorBase() noexcept = default;
  VectorBase(Allocator const& allocator) noexcept : allocator_(allocator) {}
  Allocator allocator_;
};

template <typename T, typename Allocator = Allocator<T>>
class Vector final : private VectorBase<T, Allocator> {
 public:
  using Super = VectorBase<T, Allocator>;
  using iterator = GenericIterator<T*>;
  using const_iterator = GenericIterator<T const*>;
  using reverse_iteraotr = GenericReverseIterator<iterator>;
  using const_reverse_iterator = GenericReverseIterator<const_iterator>;

  using allocator_type = Allocator;
  using allocator_traits = std::allocator_traits<allocator_type>;
  using size_type = typename allocator_traits::size_type;
  using value_type = T;
  using reference = value_type&;
  using const_reference = value_type const&;
  using difference_type = typename allocator_traits::difference_type;
  using pointer = typename allocator_traits::pointer;
  using const_pointer = typename allocator_traits::const_pointer;
  /**
   * @brief Constructor (1) Default constructor. Constructs an empty container
   * with a default-constructed allocator.
   *
   */
  Vector() noexcept(noexcept(Allocator()))
      : Super(), data_(nullptr), size_(0), capacity_(0) {}

  /**
   * @brief Constructor (2) Constructs an empty container with the given
   * allocator `allocator`.
   *
   * @param allocator
   */
  explicit Vector(Allocator const& alloc) : Super(alloc) {}

  /**
   * @brief Constructor (3) Constructs the container with n copies of
   * elements with value `value`.
   *
   * @param n
   */

  Vector(size_type n,
         const_reference value,
         allocator_type const& alloc = Allocator())
      : Super(), data_(this->allocator_.allocate(n)), size_(0), capacity_(n) {
    std::ignore = alloc;
    while (size_ != capacity_) {
      data_[size_++] = value;
    }
  }

  /**
   * @brief Constructor (4) Constructs the container with count default-inserted
   * instances of T. No copies are made.
   *
   * @param n
   */
  explicit Vector(size_type n)
      : Super(), data_(this->allocator_.allocate(n)), size_(0), capacity_(n) {
    while (size_ != capacity_) {
      base::Construct(data_ + (size_++));
    }
  }

  /**
   * @brief Constructor (5) Constructs the container with the contents of the
   * range [first, last).
   *
   * @tparam InputIterator
   * @param first
   * @param last
   * @param alloc
   */

  // TODO: Needs a real constraint for InputIterator.
  template <typename InputIterator,
            typename =
                typename std::enable_if<!std::is_integral<InputIterator>::value,
                                        InputIterator>::type>
  Vector(InputIterator first,
         InputIterator last,
         Allocator const& alloc = Allocator())
      : Super(), data_(nullptr), size_(0), capacity_(0) {
    std::ignore = alloc;
    typename base::IteratorTraits<InputIterator>::difference_type diff =
        last - first;
    if (diff <= 0)
      return;
    capacity_ = diff;
    data_ = this->allocator_.allocate(diff);
    while (first != last) {
      data_[size_++] = *first;
      ++first;
    }
  }

  /**
   * @brief Constructor (6) Constructs the container with the copy of the
   * contents of other.
   *
   * @param other
   */
  Vector(Vector const& other)
      : Super(),
        data_(this->allocator_.allocate(other.Capacity())),
        size_(0),
        capacity_(other.Capacity()) {
    for (size_type idx = 0; idx < other.Size(); ++idx) {
      data_[idx] = other.data_[idx];
      ++size_;
    }
  }

  /**
   * @brief Constructor (7) Constructs the container with the copy of the
   * contents of other, using alloc as the allocator.
   *
   * @param other
   * @param alloc
   */
  Vector(Vector const& other, Allocator const& alloc)
      : Super(alloc),
        data_(this->allocator_.allocate(other.Capacity())),
        size_(0),
        capacity_(other.Capacity()) {
    for (size_type idx = 0; idx < other.Size(); ++idx) {
      data_[idx] = other.data_[idx];
      ++size_;
    }
  }

  /**
   * @brief Constructor (8) Move constructor. Constructs the container with the
   * contents of other using move semantics. Allocator is obtained by
   * move-construction from the allocator belonging to other. After the move,
   * other is guaranteed to be empty().
   *
   * @param other
   */
  Vector(Vector&& other) noexcept
      : Super(std::move(other.GetAllocator())),
        data_(other.Data()),
        size_(other.Size()),
        capacity_(other.Capacity()) {
    other.data_ = nullptr;
    other.capacity_ = 0;
    other.size_ = 0;
  }

  /**
   * @brief Constructor (9) Allocator-extended move constructor. Using alloc as
   * the allocator for the new container, moving the contents from other; if
   * alloc != other.get_allocator(), this results in an element-wise move. (In
   * that case, other is not guaranteed to be empty after the move.)
   *
   * @param other
   * @param alloc
   */
  Vector(Vector&& other, Allocator const& alloc) noexcept
      : Super(alloc), size_(other.Size()), capacity_(other.Capacity()) {
    if (alloc != other.GetAllocator()) {
      data_ = this->allocator_.allocate(capacity_);
      for (std::size_t idx = 0ull; idx < other.Size(); ++idx) {
        data_[idx] = std::move(other.data_[idx]);
      }
    } else {
      std::swap(data_, other.data_);
    }
    other.size_ = 0;
    other.capacity_ = 0;
  }

  /**
   * @brief Constructor (10) Constructs the container with the contents of the
   * initializer list init.
   *
   * @param init
   * @param alloc
   */
  Vector(std::initializer_list<T> init, Allocator const& alloc = Allocator())
      : Super(alloc),
        data_(this->allocator_.allocate(init.size())),
        size_(0),
        capacity_(init.size()) {
    for (auto&& value : init) {
      data_[size_++] = std::move(value);
    }
  }
  /**
   * @brief Destroy the Vector object
   *
   */

  // final
  ~Vector() {
    Clear();
    this->allocator_.deallocate(data_, 1);
  }

  allocator_type GetAllocator() const noexcept { return this->allocator_; }

  // Iterators.
  iterator begin() noexcept { return iterator(data_); }
  const_iterator begin() const noexcept { return iterator(data_); }

  iterator end() noexcept { return iterator(data_ + size_); }
  const_iterator end() const noexcept { return iterator(data_ + size_); }

  const_iterator cbegin() const { return const_iterator(data_); }

  const_iterator cend() const { return const_iterator(data_ + size_); }

  reverse_iteraotr rbegin() noexcept { return reverse_iteraotr(end()); }
  const_reverse_iterator rbegin() const noexcept {
    return reverse_iteraotr(end());
  }

  reverse_iteraotr rend() noexcept { return reverse_iteraotr(begin()); }
  const_reverse_iterator rend() const noexcept {
    return reverse_iteraotr(begin());
  }

  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }

  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }
  // ~Iterators.

  // Element Access
  T& At(size_type index) { return data_[index]; }
  T const& At(size_type index) const { return data_[index]; }
  T& operator[](size_type index) { return data_[index]; }
  T const& operator[](size_type index) const { return data_[index]; }
  T& Front() { return data_[0]; }
  T const& Front() const { return data_[0]; }
  T& Back() { return data_[size_ - 1]; }
  T const& Back() const { return data_[size_ - 1]; }
  T* Data() noexcept { return data_; }
  T const* Data() const noexcept { return data_; }
  // ~Element Access
  // Size
  bool Empty() const noexcept { return size_ == 0; }
  size_type Size() const noexcept { return size_; }
  size_type MaxSize() const noexcept {
    return std::numeric_limits<size_type>::max() / sizeof(T);
  }
  size_type Capacity() const noexcept { return capacity_; }
  void ShrinkToFit() const {}
  // ~Size
  // Modifiers
  void Clear() {
    while (size_ != 0) {
      base::Destruct(data_ + --size_);
    }
  }

  iterator Insert(const_iterator const_iter, T const& t) {
    std::ignore = const_iter;
    std::ignore = t;
  }

  void Emplace(T&& t) { std::ignore = t; }

  iterator Erase() {}
  void PushBack(T const& t) {
    EnsureCapacity();
    data_[size_++] = t;
  }

  void PushBack(T&& t) {
    EnsureCapacity();
    data_[size_++] = std::move(t);
  }

  template <typename... Args>
  void EmplaceBack(Args&&... args) {
    EnsureCapacity();
    ++size_;
    base::Construct(data_ + size_, std::forward<Args>(args)...);
  }

  T PopBack() { return std::move(data_[--size_]); }
  // ~Modifiers

  void Reserve(size_type n) { std::ignore = n; }
  void Resize(size_type n, T const& t) {
    std::ignore = t;
    capacity_ = n;
    size_ = n;
    T* new_area = this->allocator_.allocate(n);
  }

 private:
  // internal funcitons.
  friend class VectorTestHelper;
  void EnsureCapacity() {
    if (!data_) {
      capacity_ = 4u;
      data_ = this->allocator_.allocate(capacity_);
      return;
    }
    if (size_ == capacity_) {
      capacity_ <<= 1;
      T* new_data = this->allocator_.allocate(capacity_);
      for (size_type i = 0; i < size_; ++i) {
        new_data[i] = std::move(data_[i]);
      }
      this->allocator_.deallocate(data_, 1);
      data_ = new_data;
    }
  }

  // data members.
  T* data_ = nullptr;
  size_type size_ = 0;
  size_type capacity_ = 0;
};
}  // namespace base
#endif