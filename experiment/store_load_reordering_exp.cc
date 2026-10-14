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

#include <semaphore.h>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "base/cxx.h"

#if SINCE(CXX_20)
int x = 0;
int y = 0;

int a = 0;
int b = 0;

std::atomic_int i;
std::atomic_int j;

std::atomic_flag flag0{false};
std::atomic_flag flag1{false};

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
#else
int x = 0, y = 0, a = 0, b = 0;
sem_t s0;
sem_t s1;
sem_t e;
void Func0() {
  while (true) {
    sem_wait(&s0);
    x = 1;
    a = y;
    sem_post(&e);
  }
}

void Func1() {
  while (true) {
    sem_wait(&s1);
    y = 1;
    b = x;
    sem_post(&e);
  }
}

int main() {
  sem_init(&s0, 0, 0);
  sem_init(&s1, 0, 0);
  sem_init(&e, 0, 0);

  std::int64_t iterations = 0;
  std::int64_t simultaneous_reordering = 0;

  std::thread thread0{Func0};
  std::thread thread1{Func1};
  while (true) {
    x = 0;
    y = 0;
    sem_post(&s0);
    sem_post(&s1);
    sem_wait(&e);
    sem_wait(&e);
    ++iterations;
    if (a == 0 && b == 0) {
      ++simultaneous_reordering;
    }
    std::cout << "Simultaneous reordering ratio = "
              << static_cast<double>(simultaneous_reordering) / iterations *
                     100.0
              << " %\n";
  }

  thread0.join();
  thread1.join();
}
#endif  // SINCE(CXX_20)