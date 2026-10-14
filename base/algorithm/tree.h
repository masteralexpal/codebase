/**
 * @file tree.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include <tuple>

namespace base {
template <typename T>
struct Node {
  T value;
  Node* l = nullptr;
  Node* r = nullptr;
  Node* p = nullptr;
};

template <typename T>
class Tree {
 public:
  using TreeNode = Node<T>;
  Tree() : root_(nullptr) {}

  void Insert(T const& value) {
    if (!root_) {
      root_ = new TreeNode();
      root_->value = value;
      return;
    }

    TreeNode* node = root_;
    return;
  }

  void Remove(T const& value) { std::ignore = value; }

  void Print() const {}

  virtual ~Tree();

 private:
  void PreOrder(TreeNode* node) {
    if (node) {
      std::cout << node->value;
      PreOrder(node->l);
      PreOrder(node->r);
    }
  }

  void InOrder(TreeNode* node) {
    if (node) {
      PreOrder(node->l);
      std::cout << node->value;
      PreOrder(node->r);
    }
  }

  void PostOrder(TreeNode* node) {
    if (node) {
      PreOrder(node->l);
      PreOrder(node->r);
      std::cout << node->value;
    }
  }

  TreeNode* root_ = nullptr;
};
}  // namespace base