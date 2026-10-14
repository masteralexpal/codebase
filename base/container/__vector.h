/**
 * @file __vector.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "base/allocator/allocator.h"
#include "base/container/internal/iterator.h"
#include "base/container/internal/vector_base.h"

namespace base {
template <typename Tp, typename Allocator = base::Allocator<Tp>>
class Vector final : public base::internal::VectorBase<Tp> {
 public:
  using value_type = Tp;
  using allocator_type = Allocator;
  using allocator_traits = std::allocator_traits<allocator_type>;
  using size_type = typename allocator_traits::size_type;
  using reference = value_type&;
  using const_reference = value_type const&;
  using difference_type = typename allocator_traits::difference_type;
  using pointer = typename allocator_traits::pointer;
  using const_pointer = typename allocator_traits::const_pointer;

  using iterator = GenericIterator<pointer>;
  using const_iterator = GenericIterator<const_pointer>;
  using reverse_iteraotr = GenericReverseIterator<iterator>;
  using const_reverse_iterator = GenericReverseIterator<const_iterator>;

  constexpr Vector() noexcept : Super() {}

 private:
  using Super = internal::VectorBase<Tp>;
};
}  // namespace base