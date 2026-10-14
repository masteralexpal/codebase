/**
 * @file iterator.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONTAINER_INTERANL_H_
#define BASE_CONTAINER_INTERANL_H_

#include <utility>

#include "base/container/internal/iterator_traits.h"
namespace base {

template <typename Iterator>
class GenericIterator {
 public:
  using iterator_type = Iterator;
  using Traits = IteratorTraits<iterator_type>;
  using iterator_category = typename Traits::iterator_category;
  using value_type = typename Traits::value_type;
  using pointer = typename Traits::pointer;
  using reference = typename Traits::reference;
  using difference_type = typename Traits::difference_type;

  /**
   * @brief Default constructor.
   *
   */
  GenericIterator() noexcept : iterator_() {}

  /**
   * @brief Constructor with const &iter.
   *
   * @param iterator - The underlying iterator.
   */
  explicit GenericIterator(iterator_type const& iterator) noexcept
      : iterator_(iterator) {}

  /**
   * @brief Copy constructor.
   *
   * @param iterator
   */
  GenericIterator(GenericIterator const& iterator) noexcept
      : iterator_(iterator.iterator_) {}

  /**
   * @brief Copy assignment.
   *
   * @param iterator
   * @return GenericIterator&
   */
  GenericIterator& operator=(GenericIterator const& iterator) noexcept {
    iterator_ = iterator.iterator_;
    return *this;
  }

  /**
   * @brief Conversion constructor.
   *
   * @tparam Iter
   * @param iterator
   */
  template <typename Iter>
  explicit GenericIterator(GenericIterator<Iter> const& iterator) noexcept
      : iterator_(iterator.base()) {}

  /**
   * @brief Conversion assignment.
   *
   * @tparam Iter
   * @param iterator
   * @return GenericIterator&
   */
  template <typename Iter>
  GenericIterator& operator=(GenericIterator<Iter> const& iterator) noexcept {
    iterator_ = iterator.iterator_;
    return *this;
  }

  iterator_type base() const noexcept { return iterator_; }

  reference operator*() const noexcept { return *iterator_; }

  pointer operator->() const noexcept { return iterator_; }

  reference operator[](difference_type diff) const noexcept {
    return iterator_[diff];
  }

  GenericIterator& operator++() noexcept {
    ++iterator_;
    return *this;
  }

  GenericIterator& operator--() noexcept {
    --iterator_;
    return *this;
  }

  GenericIterator operator++(int) noexcept {
    return GenericIterator(iterator_++);
  }

  GenericIterator operator--(int) noexcept {
    return GenericIterator(iterator_--);
  }

  GenericIterator& operator+=(difference_type diff) noexcept {
    iterator_ += diff;
    return *this;
  }

  GenericIterator& operator-=(difference_type diff) noexcept {
    iterator_ -= diff;
    return *this;
  }

  GenericIterator operator+(difference_type diff) const noexcept {
    return GenericIterator(iterator_ + diff);
  }

  GenericIterator operator-(difference_type diff) const noexcept {
    return GenericIterator(iterator_ - diff);
  }

 private:
  iterator_type iterator_;
};

template <typename Iterator>
constexpr inline bool operator==(GenericIterator<Iterator> const& lhs,
                                 GenericIterator<Iterator> const& rhs) {
  return lhs.base() == rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator==(GenericIterator<L> const& lhs,
                                 GenericIterator<R> const& rhs) {
  return lhs.base() == rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator!=(GenericIterator<L> const& lhs,
                                 GenericIterator<R> const& rhs) {
  return lhs.base() != rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator<(GenericIterator<L> const& lhs,
                                GenericIterator<R> const& rhs) {
  return lhs.base() < rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator<=(GenericIterator<L> const& lhs,
                                 GenericIterator<R> const& rhs) {
  return lhs.base() <= rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator>(GenericIterator<L> const& lhs,
                                GenericIterator<R> const& rhs) {
  return lhs.base() > rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator>=(GenericIterator<L> const& lhs,
                                 GenericIterator<R> const& rhs) {
  return lhs.base() >= rhs.base();
}

template <typename L, typename R>
constexpr inline auto operator-(GenericIterator<L> const& lhs,
                                GenericIterator<R> const& rhs)
    -> decltype(lhs.base() - rhs.base()) {
  return lhs.base() - rhs.base();
}

template <typename Iterator>
constexpr inline GenericIterator<Iterator> operator+(
    typename GenericIterator<Iterator>::difference_type diff,
    GenericIterator<Iterator> const& iterator) {
  return GenericIterator(iterator.base() + diff);
}

template <typename Iterator>
constexpr inline typename GenericIterator<Iterator>::difference_type operator-(
    GenericIterator<Iterator> const& lhs,
    GenericIterator<Iterator> const& rhs) {
  return lhs.base() - rhs.base();
}

template <typename Iterator>
class GenericReverseIterator {
 public:
  using iterator_type = Iterator;
  using Traits = IteratorTraits<iterator_type>;
  using iterator_category = typename Traits::iterator_category;
  using value_type = typename Traits::value_type;
  using pointer = typename Traits::pointer;
  using reference = typename Traits::reference;
  using difference_type = typename Traits::difference_type;

  /**
   * @brief Default constructor.
   *
   */
  GenericReverseIterator() noexcept : iterator_() {}

  /**
   * @brief Constructor from underlying iterator.
   *
   * @param iterator
   */
  GenericReverseIterator(iterator_type const& iterator) noexcept
      : iterator_(iterator) {}

  /**
   * @brief Construct a new Generic Reverse Iterator object
   *
   * @param iterator
   */
  GenericReverseIterator(GenericReverseIterator const& iterator) noexcept
      : iterator_(iterator.iterator_) {}

  GenericReverseIterator& operator=(
      GenericReverseIterator const& iterator) noexcept {
    iterator_ = iterator.iterator_;
    return *this;
  }

  /**
   * @brief Conversion constructor.
   *
   * @tparam Iter
   * @param iterator
   */
  template <typename Iter>
  GenericReverseIterator(GenericReverseIterator<Iter> const& iterator) noexcept
      : iterator_(iterator.iterator_) {}

  /**
   * @brief Conversion assignment.
   *
   * @tparam Iter
   * @param iterator
   * @return GenericReverseIterator&
   */
  template <typename Iter>
  GenericReverseIterator& operator=(
      GenericReverseIterator<Iter> const& iterator) noexcept {
    iterator_ = iterator.iterator_;
    return *this;
  }

  iterator_type base() const noexcept { return iterator_; }

  reference operator*() const noexcept {
    iterator_type tmp = iterator_;
    return *(--tmp);
  }

  pointer operator->() const noexcept {
    iterator_type tmp = iterator_;
    return (--tmp).operator->();
  }

  reference operator[](difference_type diff) const noexcept {
    return *(iterator_ - diff);
  }

  GenericReverseIterator& operator++() noexcept {
    --iterator_;
    return *this;
  }

  GenericReverseIterator& operator--() noexcept {
    ++iterator_;
    return *this;
  }

  GenericReverseIterator operator++(int) noexcept {
    iterator_type tmp = iterator_;
    --iterator_;
    return GenericReverseIterator(iterator_);
  }

  GenericReverseIterator operator--(int) noexcept {
    iterator_type tmp = iterator_;
    ++tmp;
    return tmp;
  }

  GenericReverseIterator& operator+=(difference_type diff) noexcept {
    iterator_ -= diff;
    return *this;
  }

  GenericReverseIterator& operator-=(difference_type diff) noexcept {
    iterator_ += diff;
    return *this;
  }

  GenericReverseIterator operator+(difference_type diff) const noexcept {
    return GenericReverseIterator(iterator_ - diff);
  }

  GenericReverseIterator operator-(difference_type diff) const noexcept {
    return GenericReverseIterator(iterator_ + diff);
  }

 private:
  iterator_type iterator_;
};

template <typename Iterator>
constexpr inline bool operator==(GenericReverseIterator<Iterator> const& lhs,
                                 GenericReverseIterator<Iterator> const& rhs) {
  return lhs.base() == rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator==(GenericReverseIterator<L> const& lhs,
                                 GenericReverseIterator<R> const& rhs) {
  return lhs.base() == rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator!=(GenericReverseIterator<L> const& lhs,
                                 GenericReverseIterator<R> const& rhs) {
  return lhs.base() != rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator<(GenericReverseIterator<L> const& lhs,
                                GenericReverseIterator<R> const& rhs) {
  return lhs.base() > rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator<=(GenericReverseIterator<L> const& lhs,
                                 GenericReverseIterator<R> const& rhs) {
  return lhs.base() >= rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator>(GenericReverseIterator<L> const& lhs,
                                GenericReverseIterator<R> const& rhs) {
  return lhs.base() < rhs.base();
}

template <typename L, typename R>
constexpr inline bool operator>=(GenericReverseIterator<L> const& lhs,
                                 GenericReverseIterator<R> const& rhs) {
  return lhs.base() <= rhs.base();
}

template <typename L, typename R>
constexpr inline auto operator-(GenericReverseIterator<L> const& lhs,
                                GenericReverseIterator<R> const& rhs)
    -> decltype(rhs.base() - lhs.base()) {
  return rhs.base() - lhs.base();
}

template <typename Iterator>
constexpr inline GenericReverseIterator<Iterator> operator+(
    typename GenericReverseIterator<Iterator>::difference_type diff,
    GenericReverseIterator<Iterator> const& iterator) {
  return GenericReverseIterator(iterator.base() - diff);
}

template <typename Iterator>
constexpr inline GenericReverseIterator<Iterator> operator-(
    typename GenericReverseIterator<Iterator>::difference_type diff,
    GenericReverseIterator<Iterator> const& iterator) {
  return GenericReverseIterator(iterator.base() + diff);
}

}  // namespace base
#endif