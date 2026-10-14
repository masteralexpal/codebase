/**
 * @file iterator_traits.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CONTAINER_INTERNAL_ITERATOR_TRAITS_H_
#define BASE_CONTAINER_INTERNAL_ITERATOR_TRAITS_H_

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace base {

/**
 * @brief The tag of LegacyInputIterator
 *
 */
struct InputIteratorTag {};

/**
 * @brief The tag of LegacyOutputIterator
 *
 */
struct OutputIteratorTag {};

/**
 * @brief The tag of LegacyForwardIteratorTag
 *
 */
struct ForwardIteratorTag : public InputIteratorTag {};

/**
 * @brief The tag of LegacyBidirectionalIteratorTag
 *
 */
struct BidirectionalIteratorTag : public ForwardIteratorTag {};
/**
 * @brief The tag of LegacyRandomAccessIteratorTag
 *
 */

struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};

template <typename Category,
          typename T,
          typename Diff = ptrdiff_t,
          typename Ptr = T*,
          typename Ref = T&>
struct IteratorBase {
  using iterator_category = Category;
  using value_type = T;
  using difference_type = Diff;
  using pointer = Ptr;
  using reference = Ref;
};

template <typename Iterator>
struct IteratorTraits {
  using iterator_category = typename Iterator::iterator_category;
  using value_type = typename Iterator::value_type;
  using difference_type = typename Iterator::difference_type;
  using pointer = typename Iterator::pointer;
  using reference = typename Iterator::reference;
};

/**
 * @brief using for extracting traits from pointer type.
 *
 * @tparam T - value type.
 */
template <typename T>
struct IteratorTraits<T*> {
  using iterator_category = RandomAccessIteratorTag;
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using difference_type = std::ptrdiff_t;
};

/**
 * @brief using for extracting traits from pointer-to-const type.
 *
 * @tparam T
 */
template <typename T>
struct IteratorTraits<T const*> {
  using iterator_category = RandomAccessIteratorTag;
  using value_type = T;
  using pointer = T const*;
  using reference = T const&;
  using difference_type = std::ptrdiff_t;
};

template <typename Iterator, typename = void>
struct IsInputIterator {
  constexpr static bool value = false;
};

template <typename Iterator>
struct IsInputIterator<
    Iterator,
    std::enable_if<std::is_base_of<base::InputIteratorTag,
                                   typename base::IteratorTraits<
                                       Iterator>::iterator_category>::value,
                   Iterator>> {
  constexpr static bool value = true;
};

template <typename Iterator>
struct IsInputIterator<
    Iterator,
    std::enable_if<std::is_base_of<std::input_iterator_tag,
                                   typename std::iterator_traits<
                                       Iterator>::iterator_category>::value,
                   Iterator>> {
  constexpr static bool value = true;
};

}  // namespace base
#endif