/**
 * @file linked_symbol_table.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-10-02
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_ALGORITHM_LINKED_SYMBOL_TABLE_H_
#define BASE_ALGORITHM_LINKED_SYMBOL_TABLE_H_
#include <utility>
#include "base/algorithm/symbol_table.h"
namespace base {
template <typename K, typename V>
class LinkedSymbolTable : public SymbolTable<K, V> {
 public:
  LinkedSymbolTable() {}

  bool Put(K const& key, V const& value) override {
    if (!root) {
      root = new Node(key, value, nullptr);
      return true;
    }
    Node* current = root;
    while (current->next) {
      if (current->data.first == key) {
        current->data.second = value;
        break;
      }
      current = current->next;
    }
  }

 private:
  using Data = std::pair<K, V>;
  struct Node {
    Data data = {};
    Node* next = nullptr;
  };

  Node* root = nullptr;
};
}  // namespace base
#endif