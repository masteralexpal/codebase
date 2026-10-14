/**
 * @file enable_if_exp.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include <type_traits>

class A {};

class B : public A {};

class C {};

template <
    typename T,
    typename std::enable_if<std::is_base_of<A, T>::value, bool>::type = true>
void Function(T const& val) {
  std::cout << "Only A or A's derived classes are permitted." << std::endl;
}

template <
    typename T,
    typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
void Function(T const& val) {
  std::cout << "Only integer types are permitted." << std::endl;
}

int main() {
  A a;
  B b;
  C c;
  Function(a);
  Function(b);
  // Function(c); // Error
}
