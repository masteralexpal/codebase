/**
 * @file hash.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_ALGORITHM_H_
#define BASE_ALGORITHM_H_

#include <cstdint>
#include <string>

namespace base {

template <typename Char>
class FNV1A32 {
 public:
  std::uint32_t const fnv_prime = 16777619u;
  std::uint32_t const fnv_offset_basis = 2166136261u;

  std::uint32_t operator()(std::basic_string<Char> const& str) {
    std::uint32_t hash = fnv_offset_basis;
    for (auto&& c : str) {
      hash ^= c;
      hash *= fnv_prime;
    }
    return hash;
  }
};

template <typename Char>
class FNV1A64 {
 public:
  std::uint64_t const fnv_prime = 1099511628211u;
  std::uint64_t const fnv_offset_basis = 14695981039346656037u;

  std::uint64_t operator()(std::basic_string<Char> const& str) {
    std::uint64_t hash = fnv_offset_basis;
    for (auto&& c : str) {
      hash ^= c;
      hash *= fnv_prime;
    }
    return hash;
  }
};

template <typename Integer>
struct IntegerTraits {};

template <>
struct IntegerTraits<std::uint32_t> {
  std::size_t static constexpr fnv_prime = 16777619u;
  std::size_t static constexpr fnv_offset_basis = 2166136261u;
};

template <>
struct IntegerTraits<std::uint64_t> {
  std::size_t static constexpr fnv_prime = 1099511628211u;
  std::size_t static constexpr fnv_offset_basis = 14695981039346656037u;
};

template <typename String, typename Integer>
class FNV1A {
 public:
  Integer operator()(String const& str) {
    Integer hash = IntegerTraits<Integer>::fnv_offset_basis;
    for (auto&& c : str) {
      hash ^= c;
      hash *= IntegerTraits<Integer>::fnv_prime;
    }
    return hash;
  };
};

// For 32 bit machines:
// const std::size_t fnv_prime = 16777619u;
// const std::size_t fnv_offset_basis = 2166136261u;

// For 64 bit machines:
// const std::size_t fnv_prime = 1099511628211u;
// const std::size_t fnv_offset_basis = 14695981039346656037u;

// For 128 bit machines:
// const std::size_t fnv_prime = 309485009821345068724781401u;
// const std::size_t fnv_offset_basis =
//     275519064689413815358837431229664493455u;

// For 256 bit machines:
// const std::size_t fnv_prime =
//     374144419156711147060143317175368453031918731002211u;
// const std::size_t fnv_offset_basis =
//     100029257958052580907070968620625704837092796014241193945225284501741471925557u;

}  // namespace base
#endif  // BASE_ALGORITHM_H_