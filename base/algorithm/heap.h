/**
 * @file heap.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_ALGORITHM_HEAP_H_
#define BASE_ALGORITHM_HEAP_H_
#include <cstddef>
#include <type_traits>
#include <utility>
#include <variant>

#include "base/algorithm/comparator.h"
namespace base {
template <typename T, typename Comparator>
class Heap {
 public:
  Heap(std::size_t initial_size)
      : data_(new T[initial_size + 1]), size_(0), capacity_(initial_size + 1) {}

  // copy constructor.
  Heap(Heap const& heap)
      : data_(new T[heap.capacity_]),
        size_(heap.size_),
        capacity_(heap.capacity_),
        comparator_(heap.comparator_) {
    for (std::size_t i = 1; i <= size_; ++size_) {
      // copy.
      data_[i] = heap.data_[i];
    }
  }

  // copy assigment.
  // copy and sawp.
  Heap& operator=(Heap const& heap) {
    if (this != &heap) {
      Heap temp = heap;
      Swap(temp);
    }
    return *this;
  }

  // move constructor.
  Heap(Heap&& heap) noexcept
      : data_(heap.data_),
        size_(heap.size_),
        capacity_(heap.capacity_),
        comparator_(std::move(comparator_)) {
    heap.data_ = nullptr;
    heap.size_ = 0u;
    heap.capacity_ = 0u;
  }
  // move assigment.
  Heap& operator=(Heap&& heap) noexcept {
    // swap the data_ to heap,
    // and then heap's destructor will free data_.
    std::swap(data_, heap.data_);
    std::swap(size_, heap.size_);
    std::swap(capacity_, heap.capacity_);
    capacity_ = std::move(heap.comparator_);
  }

  std::size_t Size() const { return size_; }

  bool Empty() const { return size_ == 0u; }

  // void Push(T &&value) {}

  void Push(T const& value) {
    data_[++size_] = value;
    Swin(size_);
    if (size_ == capacity_) {
      Extend();
    }
  }

  T Pop() {
    T poped = std::move(data_[1]);
    Exchange(1, size_--);
    data_[size_ + 1] = {};
    Sink(1);
    return poped;
  }

  virtual ~Heap() { delete[] data_; }

 private:
  void Swap(Heap& heap) {
    std::swap(data_, heap.data_);
    std::swap(size_, heap.size_);
    std::swap(capacity_, heap.capacity_);
    std::swap(comparator_, heap.comparator_);
  }

  void Exchange(std::size_t a, std::size_t b) {
    T tmp = data_[a];
    data_[a] = data_[b];
    data_[b] = std::move(tmp);
  }

  void Swin(std::size_t index) {
    while (index > 1 && comparator_(data_[index / 2], data_[index])) {
      Exchange(index / 2, index);
      index = index / 2;
    }
  }

  void Sink(std::size_t index) {
    while (2 * index <= size_) {
      auto j = 2 * index;
      if (j < size_ && comparator_(data_[j], data_[j + 1])) {
        ++j;
      }
      if (!comparator_(data_[index], data_[j])) {
        break;
      }
      Exchange(index, j);
      index = j;
    }
  }

  void Extend() {}

  T* data_ = nullptr;          // T[0] is not used.
  std::size_t size_ = 0u;      // element count.
  std::size_t capacity_ = 0u;  // data_ size.
  Comparator comparator_;      // element comparetor.
};

template <typename T>
using MaxHeap = Heap<T, LessThan<T, T>>;

template <typename T>
using MinHeap = Heap<T, GreaterThan<T, T>>;
}  // namespace base
#endif