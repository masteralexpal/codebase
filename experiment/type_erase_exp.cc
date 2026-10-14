/**
 * @file type_erase_exp.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <functional>
#include <iostream>
#include <tuple>
#include <type_traits>

// template <typename Callable, typename... Args> class Closure {
// public:
//   using BoundArgs = std::tuple<Args...>;

// private:
//   Callable callable_;
//   BoundArgs args_;
// };

// struct A {
//   void operator()() { std::cout << "A::operator() called!" << std::endl; }
// };

// void TestFunction() {
//   A a;
//   std::function<void()> function_0{a};
//   std::function<void()> function_1{
//       []() -> void { std::cout << "Lambda called!" << std::endl; }};
//   function_0();
//   function_1();
// }

struct DeleterBase {
  virtual void Delete(void* ptr) = 0;
  virtual ~DeleterBase() { std::cout << "~DeleterBase()" << std::endl; }
};

template <typename Type, typename Deleter>
struct DeleterImpl : public DeleterBase {
  DeleterImpl(Deleter deleter) : deleter_(std::move(deleter)) {}

  void Delete(void* ptr) override { deleter_(static_cast<Type*>(ptr)); }

  ~DeleterImpl() override { std::cout << "~DeleterImpl()" << std::endl; }

  Deleter deleter_;
};

void Delete(int* ptr) {
  std::cout << "delete by plain function." << std::endl;
  delete ptr;
}
int main() {
  auto lambda = [](int* ptr) -> void {
    std::cout << "delete by lambda." << std::endl;
    delete (ptr);
  };
  DeleterBase* d = new DeleterImpl<int, decltype(lambda)>(lambda);
  int* x = new int(1);
  d->Delete(x);
  delete d;
  d = new DeleterImpl<int, void (*)(int* ptr)>(Delete);
  int* y = new int(2);
  d->Delete(y);
  delete d;
}