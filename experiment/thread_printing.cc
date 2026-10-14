/**
 * @file thread_printing.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

namespace a {
std::mutex mtx;
std::condition_variable cv;
int counter = 0;
inline constexpr int kMax = 30;
void print_a() {
  while (true) {
    {
      std::lock_guard<std::mutex> lock_guard{mtx};
      if (!(counter < kMax))
        break;
      if (counter % 4 == 0) {
        std::cout << "thread 0, print: A\n";
        ++counter;
      }
    }
    // notify_one() is enough.
    cv.notify_one();
  }
}

void print_b() {
  while (true) {
    std::unique_lock<std::mutex> unique_lock{mtx};
    if (!(counter < kMax))
      break;
    while (!(counter % 4 == 1)) {
      cv.wait(unique_lock);
    }
    std::cout << "thread 1, print: B\n";
    ++counter;
    unique_lock.unlock();
    // notify_one() is enough.
    cv.notify_one();
  }
}

void print_c() {
  while (true) {
    std::unique_lock<std::mutex> unique_lock{mtx};
    if (!(counter < kMax))
      break;
    while (!(counter % 4 == 2)) {
      cv.wait(unique_lock);
    }
    std::cout << "thread 2, print: C\n";
    ++counter;
    unique_lock.unlock();
    cv.notify_one();
  }
}

void print_d() {
  while (true) {
    std::unique_lock<std::mutex> unique_lock{mtx};
    if (!(counter < kMax))
      break;
    while (!(counter % 4 == 3)) {
      cv.wait(unique_lock);
    }
    std::cout << "thread 3, print: D\n";
    ++counter;
    unique_lock.unlock();
    cv.notify_one();
  }
}

void a() {
  std::thread thread0{print_a};
  std::thread thread1{print_b};
  std::thread thread2{print_c};
  std::thread thread3{print_d};

  thread0.join();
  thread1.join();
  thread2.join();
  thread3.join();
}
}  // namespace a

namespace b {

std::mutex mtx;
std::condition_variable cv;
inline constexpr int kMax = 100;
int count = 0;

void print(int value) {
  while (true) {
    {
      std::unique_lock<std::mutex> unique_lock{mtx};
      if (!(count < kMax))
        break;
      while (count % 4 != value) {
        cv.wait(unique_lock);
      }
      std::cout << "thread " << (count % 4) << ", " << 'A' + (count % 4)
                << "\n";
      ++count;
    }
    cv.notify_all();
  }
}

void b() {
  std::thread thread3{print, 3};
  std::thread thread2{print, 2};
  std::thread thread1{print, 1};
  std::thread thread0{print, 0};

  thread0.join();
  thread1.join();
  thread2.join();
  thread3.join();
}
}  // namespace b

namespace c {
std::mutex mtx;
std::condition_variable cv;
inline constexpr int kMax = 100;
int counter = 0;

void print_a() {
  while (true) {
    std::unique_lock<std::mutex> unique_lock{mtx};
    if (!(counter < kMax))
      break;
    while (!(counter % 4 == 0)) {
      cv.wait(unique_lock);
    }
    std::cout << "thread 0, print: A\n";
    ++counter;
    unique_lock.unlock();
    // notify_all() is required.
    cv.notify_all();
  }
}

void print_b() {
  while (true) {
    std::unique_lock<std::mutex> unique_lock{mtx};
    if (!(counter < kMax))
      break;
    while (!(counter % 4 == 1)) {
      cv.wait(unique_lock);
    }
    std::cout << "thread 1, print: B\n";
    ++counter;
    unique_lock.unlock();
    // notify_one() is enough.
    cv.notify_all();
  }
}

void print_c() {
  while (true) {
    std::unique_lock<std::mutex> unique_lock{mtx};
    if (!(counter < kMax))
      break;
    while (!(counter % 4 == 2)) {
      cv.wait(unique_lock);
    }
    std::cout << "thread 2, print: C\n";
    ++counter;
    unique_lock.unlock();
    cv.notify_all();
  }
}

void print_d() {
  while (true) {
    std::unique_lock<std::mutex> unique_lock{mtx};
    if (!(counter < kMax))
      break;
    while (!(counter % 4 == 3)) {
      cv.wait(unique_lock);
    }
    std::cout << "thread 3, print: D\n";
    ++counter;
    unique_lock.unlock();
    cv.notify_all();
  }
}

void c() {
  std::thread thread0{print_a};
  std::thread thread1{print_b};
  std::thread thread2{print_c};
  std::thread thread3{print_d};

  thread0.join();
  thread1.join();
  thread2.join();
  thread3.join();
}
}  // namespace c

int main() {
  a::a();
  b::b();
  c::c();
  return 0;
}