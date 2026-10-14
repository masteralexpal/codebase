/**
 * @file pointer_iterator.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONTAINER_INTERNAL_POINTER_ITERATOR_H_
#define BASE_CONTAINER_INTERNAL_POINTER_ITERATOR_H_

#include <cstddef>

#include "base/container/internal/iterator_traits.h"

namespace base {

/**
 * @brief A iterator to wrap pointer.
 *
 */
template <typename T, typename Container>
class PointerIterator {
 public:
  using P = T*;
  using iterator_category = typename IteratorTraits<P>::iterator_category;
  using value_type = typename IteratorTraits<P>::value_type;
  using pointer = typename IteratorTraits<P>::pointer;
  using reference = typename IteratorTraits<P>::reference;
  using difference_type = typename IteratorTraits<P>::difference_type;

  PointerIterator() noexcept = default;
  PointerIterator(T* p) noexcept : p_(p) {}

  // `const` is needed, becauese the constancy of iterator has
  // noting to do with T.
  reference operator*() const noexcept { return *p_; }

  pointer operator->() const noexcept { return p_; }

  PointerIterator& operator++() noexcept {
    ++p_;
    return *this;
  }

  PointerIterator& operator--() noexcept {
    --p_;
    return *this;
  }

  PointerIterator operator++(int) noexcept {
    auto tmp = p_;
    ++p_;
    return PointerIterator(p_);
  }

  PointerIterator operator--(int) noexcept {
    auto tmp = p_;
    --p_;
    return PointerIterator(p_);
  }

  PointerIterator& operator+=(difference_type diff) noexcept {
    p_ += diff;
    return *this;
  }

  PointerIterator& operator-=(difference_type diff) noexcept {
    p_ -= diff;
    return *this;
  }

  PointerIterator operator+(difference_type diff) const noexcept {
    return PointerIterator{p_ + diff};
  }

  PointerIterator operator-(difference_type diff) const noexcept {
    return PointerIterator{p_ - diff};
  }

  bool operator==(PointerIterator const& other) const noexcept {
    return p_ == other.p_;
  }

  bool operator!=(PointerIterator const& other) const noexcept {
    return !(*this == other);
  }

  reference operator[](difference_type diff) const noexcept { return p_[diff]; }

 private:
  template <typename X, typename Y, typename C>
  friend bool operator==(PointerIterator<X, C> const&,
                         PointerIterator<Y, C> const&) noexcept;

  template <typename X, typename Y, typename C>
  friend bool operator!=(PointerIterator<X, C> const&,
                         PointerIterator<Y, C> const&) noexcept;

  template <typename X, typename Y, typename C>
  friend bool operator<(PointerIterator<X, C> const&,
                        PointerIterator<Y, C> const&) noexcept;

  template <typename X, typename Y, typename C>
  friend bool operator<=(PointerIterator<X, C> const&,
                         PointerIterator<Y, C> const&) noexcept;

  template <typename X, typename Y, typename C>
  friend bool operator>(PointerIterator<X, C> const&,
                        PointerIterator<Y, C> const&) noexcept;

  template <typename X, typename Y, typename C>
  friend bool operator>=(PointerIterator<X, C> const&,
                         PointerIterator<Y, C> const&) noexcept;

  template <typename X, typename C>
  friend typename PointerIterator<X, C>::difference_type operator-(
      PointerIterator<X, C> const&,
      PointerIterator<X, C> const&) noexcept;

  template <typename X, typename Y, typename C>
  friend inline auto operator-(PointerIterator<X, C> const& lhs,
                               PointerIterator<Y, C> const& rhs) noexcept
      -> decltype(lhs.p_ - rhs.p_);

  template <typename X, typename C>
  PointerIterator<X, C> friend operator+(
      typename PointerIterator<X, C>::difference_type diff,
      PointerIterator<X, C> const& iter) noexcept;

 private:
  pointer p_ = nullptr;
};

template <typename T, typename U, typename Container>
inline bool operator==(PointerIterator<T, Container> const& lhs,
                       PointerIterator<U, Container> const& rhs) noexcept {
  return lhs.p_ == rhs.p_;
}

template <typename T, typename U, typename Container>
inline bool operator!=(PointerIterator<T, Container> const& lhs,
                       PointerIterator<U, Container> const& rhs) noexcept {
  return lhs.p_ != rhs.p_;
}

template <typename T, typename U, typename Container>
inline bool operator<(PointerIterator<T, Container> const& lhs,
                      PointerIterator<U, Container> const& rhs) noexcept {
  return lhs.p_ < rhs.p_;
}

template <typename T, typename U, typename Container>
inline bool operator<=(PointerIterator<T, Container> const& lhs,
                       PointerIterator<U, Container> const& rhs) noexcept {
  return lhs.p_ <= rhs.p_;
}

template <typename T, typename U, typename Container>
inline bool operator>(PointerIterator<T, Container> const& lhs,
                      PointerIterator<U, Container> const& rhs) noexcept {
  return lhs.p_ > rhs.p_;
}

template <typename T, typename U, typename Container>
inline bool operator>=(PointerIterator<T, Container> const& lhs,
                       PointerIterator<U, Container> const& rhs) noexcept {
  return lhs.p_ >= rhs.p_;
}

template <typename T, typename Container>
inline typename PointerIterator<T, Container>::difference_type operator-(
    PointerIterator<T, Container> const& lhs,
    PointerIterator<T, Container> const& rhs) noexcept {
  return lhs.p_ - rhs.p_;
}

template <typename T, typename U, typename Container>
inline auto operator-(PointerIterator<T, Container> const& lhs,
                      PointerIterator<U, Container> const& rhs) noexcept
    -> decltype(lhs.p_ - rhs.p_) {
  return lhs.p_ - rhs.p_;
}

template <typename T, typename Container>
inline PointerIterator<T, Container> operator+(
    typename PointerIterator<T, Container>::difference_type diff,
    PointerIterator<T, Container> const& iter) noexcept {
  return {iter.p_ + diff};
}
}  // namespace base
#endif