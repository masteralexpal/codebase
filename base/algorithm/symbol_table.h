/**
 * @file symbol_table.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-10-02
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_ALGORITHM_SYMBOL_TABLE_H_
#define BASE_ALGORITHM_SYMBOL_TABLE_H_

#include <cstddef>
#include <optional>
#include <set>

namespace base {
template <typename K, typename V>
class SymbolTable {
 public:
  SymbolTable() = default;

  virtual ~SymbolTable() = default;

  virtual bool Put(K const& key, V const& value) = 0;

  virtual std::optional<V> Get(K const& key) = 0;

  virtual bool Delete(K const& key) = 0;

  virtual bool Contains(K const& key) const = 0;

  virtual bool IsEmpty(K const& key) const = 0;

  virtual std::size_t Size() const = 0;

  virtual std::set<K> Keys() const = 0;
};
}  // namespace base
#endif