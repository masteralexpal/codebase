/**
 * @file store_load_reordering_exp.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <atomic>
#include <iostream>
#include <thread>

int x = 0;
int y = 0;

int a = 0;
int b = 0;

// C++20 default constructor initializes to the clear state; do not use
// atomic_flag{false} (P0883 ctor), which MSVC STL does not implement.
std::atomic_flag flag0;
std::atomic_flag flag1;

void Func0() {
  while (true) {
    if (!flag0.test(std::memory_order_acquire)) {
      x = 1;
      // __asm__ __volatile__("dmb sy");
      a = y;
      flag0.test_and_set(std::memory_order_release);
    }
  }
}

void Func1() {
  while (true) {
    if (!flag1.test(std::memory_order_acquire)) {
      y = 1;
      // __asm__ __volatile__("dmb sy");
      b = x;
      flag1.test_and_set(std::memory_order_release);
    }
  }
}

int main() {
  std::thread thread0{Func0};
  std::thread thread1{Func1};
  int iterations = 0;
  int reordering = 0;
  while (true) {
    if (flag0.test(std::memory_order_acquire) &&
        flag1.test(std::memory_order_acquire)) {
      ++iterations;
      if (a == 0 && b == 0) {
        ++reordering;
      }
      x = 0;
      y = 0;
      a = 0;
      b = 0;
      flag0.clear(std::memory_order_release);
      flag1.clear(std::memory_order_release);
      std::cout << "Reordering ratio = "
                << reordering / static_cast<double>(iterations) * 100
                << " %\n ";
    }
  }

  thread0.join();
  thread1.join();
}
