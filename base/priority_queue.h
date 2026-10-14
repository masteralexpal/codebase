/**
 * @file priority_queue.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_PRIORITY_QUEUE_H_
#define BASE_PRIORITY_QUEUE_H_

#include <cstddef>
#include <list>
#include <string>

namespace base {
template <typename T>
class PriorityQueue {
 public:
  PriorityQueue() = default;

  virtual ~PriorityQueue() = default;

  void Insert(T const& t) {
    auto iter = list_.begin();
    while (iter != list_.end() && t < *iter) {
      ++iter;
    }
    list_.insert(iter, t);
  }

  void PopMax() { list_.pop_front(); }

  T& Max() { return list_.front(); }

  T const& Max() const { return list_.front(); }

  std::size_t Size() const noexcept { return list_.size(); }

  bool Empty() const noexcept { return list_.empty(); }

 private:
  std::list<T> list_;
};

template <typename T>
class MaxHeap {
 public:
  explicit MaxHeap(std::size_t size) : nodes_(new T[size + 1]) {}

  virtual ~MaxHeap() { delete[] nodes_; }

  bool Empty() const { return size_ == 0; }

  std::size_t Size() const { return size_; }

  void Insert(T const& t) {
    nodes_[++size_] = t;
    Swim(size_);
  }

  T PopMax() {
    T max = nodes_[1];
    Exchange(1, size_--);
    nodes_[size_ + 1] = {};
    Sink(1);
    return max;
  }

 private:
  bool Less(std::size_t i, std::size_t j) { return nodes_[i] < nodes_[j]; }

  void Exchange(std::size_t i, std::size_t j) {
    T t = nodes_[i];
    nodes_[i] = nodes_[j];
    nodes_[j] = t;
  }

  void Swim(std::size_t i) {
    while (i > 1 && Less(i / 2, i)) {
      Exchange(i / 2, i);
      i = i / 2;
    }
  }

  void Sink(std::size_t i) {
    while (2 * i <= size_) {
      auto j = 2 * i;
      if (j < size_ && Less(j, j + 1)) {
        ++j;
      }
      if (!Less(i, j)) {
        break;
      }
      Exchange(i, j);
      i = j;
    }
  }

  T* nodes_;
  std::size_t size_ = 0;
};

template <typename T>
class MinHeap {};
}  // namespace base
#endif  // BASE_PRIORITY_QUEUE_H_