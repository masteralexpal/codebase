/**
 * @file store_store_reordering_exp.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cassert>
#include <thread>

int a = 0;
int b = 0;

void func0() {
  a = 1;
  b = 1;
}

void func1() {
  while (b == 0)
    continue;
  assert(a == 1);  // Never fires on x86, but it will fail on ARM.
}

int main() {
  std::thread thread0{func0};
  std::thread thread1{func1};

  thread0.join();
  thread1.join();
}