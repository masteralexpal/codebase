/**
 * @file thread_experiments.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <tuple>
#include <utility>

int thread_entry_point(void* (*start_routine)(void* args), void* args) {
  start_routine(args);
  return 0;
}

// experiment 1.
void* start(void* args) {
  std::function<void()>* functor = static_cast<std::function<void()>*>(args);
  if (functor) {
    functor->operator()();
  }
  return nullptr;
}

void experiment1() {
  std::function<void()> f =
      std::bind([]() -> void { std::cout << "hello!" << std::endl; });
  thread_entry_point(start, static_cast<void*>(&f));
}

// experiment 2. TODO
template <typename... Args>
struct ArgumentsPack {
  using Tuple = std::tuple<Args...>;
  ArgumentsPack(Args&&... args) : tuple(std::forward<Args>(args)...) {}
  Tuple tuple;
};

template <typename... Args>
void Wrapper(Args&&... args) {
  ArgumentsPack ap{std::forward_as_tuple(args...)};
}
void experiment2() {}

template <typename F, typename... Args>
void Start(F&& f, Args&&... args) {
  std::ignore(f);
  std::ignore(args...);
}

int main() {
  experiment1();
  experiment2();
  std::this_thread::sleep_for(std::chrono::seconds{5});
  std::this_thread::sleep_until(std::chrono::system_clock::now() +
                                std::chrono::seconds{5});
  return 0;
}