/**
 * @file hash_exp.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>

static constexpr std::uint64_t const kFnvPrime64 = 1099511628211u;
static constexpr std::uint64_t const kFnvOffsetBasis64 = 14695981039346656037u;

template <typename Type>
struct FNV1A64 {
  inline std::uint64_t constexpr operator()(Type const& value) {
    auto& array = reinterpret_cast<char const(&)[sizeof(value)]>(value);
    std::uint64_t hash = kFnvOffsetBasis64;
    for (auto&& c : array) {
      hash ^= c;
      hash *= kFnvPrime64;
    }
    return hash;
  }
};

class Foo {
 public:
  Foo() = default;
  virtual ~Foo() = default;
};

int main() {
  Foo foo;
  FNV1A64<Foo> foo_hash;
  std::cout << foo_hash(foo) << std::endl;

  std::shared_ptr<int> pi = std::make_shared<int>(5);
  FNV1A64<std::shared_ptr<int>> shared_ptr_hash;
  std::cout << shared_ptr_hash(pi) << std::endl;
}