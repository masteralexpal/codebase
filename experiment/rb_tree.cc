/**
 * @file rb_tree.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include <queue>
#include <stack>

/**
 * @brief Colors of tree nodes.
 *
 */
enum Color { RED, BLACK };
/**
 * @brief
 *
 * @tparam T
 */
template <typename T>
struct Node {
  T data;        // data
  Color color;   // cloor
  Node* left;    // left child
  Node* right;   // right child
  Node* parent;  // parent

  Node(T const& val)
      : data(val), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

template <typename T>
class RedBlackTree {
 public:
  using TreeNode = Node<T>;
  RedBlackTree() : root_(nullptr) {}

  RedBlackTree(RedBlackTree const&) = delete;
  RedBlackTree& operator=(RedBlackTree const&) = delete;
  RedBlackTree(RedBlackTree&&) = delete;
  RedBlackTree& operator=(RedBlackTree&&) = delete;

  virtual ~RedBlackTree() = default;
  /**
   * @brief Insert
   *
   * @param val
   */
  void Insert(T const& val) {
    TreeNode* new_node = new TreeNode(val);
    TreeNode* parent = nullptr;
    TreeNode* current = root_;

    // find a position to insert.
    while (current) {
      parent = current;
      if (val < current->data) {
        current = current->left;
      } else {
        current = current->right;
      }
    }

    new_node->parent = parent;
    // if root is empty then new node is the root.
    if (!parent) {
      root_ = new_node;
    } else if (val < parent->data) {
      // insert at left
      parent->left = new_node;
    } else {
      // insert at right.
      parent->right = new_node;
    }

    FixInsertion(new_node);
  }

  // Refs:
  // 1.
  // https://www.enjoyalgorithms.com/blog/iterative-binary-tree-traversals-using-stack
  // 2. https://blog.csdn.net/xiaogaotongxue__/article/details/125627746
  void PreOrder() const {
    PreOrder(root_);
    std::cout << '\n';
  }

  void InOrder() const {
    InOrder(root_);
    std::cout << '\n';
  }

  void PostOrder() const {
    PostOrder(root_);
    std::cout << '\n';
  }

  void PreOrderIterative() const {
    if (!root_) {
      return;
    }
    std::stack<TreeNode*> stack;
    TreeNode* current = root_;
    stack.push(current);
    while (!stack.empty()) {
      current = stack.top();
      stack.pop();
      std::cout << current->data << ", ";
      if (current->right)
        stack.push(current->right);
      if (current->left)
        stack.push(current->left);
    }
    std::cout << '\n';
  }

  void InOrderIterative() const {
    if (!root_) {
      return;
    }
    std::stack<TreeNode*> stack;
    TreeNode* current = root_;
    while (!stack.empty() || current) {
      if (current) {
        stack.push(current);
        current = current->left;
      } else {
        current = stack.top();
        stack.pop();
        std::cout << current->data << ", ";
        current = current->right;
      }
    }
    std::cout << "\n";
  }

  void PostOrderIterative() const {
    if (!root_) {
      return;
    }
    std::stack<TreeNode*> main_stack;
    std::stack<TreeNode*> right_child_stack;
    TreeNode* current = root_;
    while (!main_stack.empty() || current) {
      if (current) {
        if (current->right) {
          right_child_stack.push(current->right);
        }
        main_stack.push(current);
        current = current->left;
      } else {
        current = main_stack.top();
        if (!right_child_stack.empty() &&
            current->right == right_child_stack.top()) {
          current = right_child_stack.top();
          right_child_stack.pop();
        } else {
          std::cout << current->data << ", ";
          main_stack.pop();
          current = nullptr;
        }
      }
    }
    std::cout << "\n";
  }

  void PostOrderIterativeSingleStack() {
    if (!root_) {
      return;
    }

    std::stack<TreeNode*> stack;
    TreeNode* current = root_;
    TreeNode* previous = nullptr;
    while (!stack.empty() || current) {
      if (current) {
        stack.push(current);
        current = current->left;
      } else {
        TreeNode* top_node = stack.top();
        if (top_node->right && top_node->right != previous) {
          current = top_node->right;
        } else {
          std::cout << top_node->data << ", ";
          previous = top_node;
          stack.pop();
        }
      }
    }
    std::cout << "\n";
  }

  void LevelOrder() {
    if (!root_) {
      return;
    }
    std::queue<TreeNode*> queue;
    queue.push(root_);
    while (!queue.empty()) {
      TreeNode* node = queue.front();
      queue.pop();
      std::cout << node->data << ", ";
      if (node->left) {
        queue.push(node->left);
      }
      if (node->right) {
        queue.push(node->right);
      }
    }
  }

 private:
  TreeNode* root_;

  void RotateLeft(TreeNode* node) {
    TreeNode* right_child = node->right;
    node->right = right_child->left;
    if (right_child->left) {
      right_child->left->parent = node;
    }
    right_child->parent = node->parent;
    if (!node->parent) {
      root_ = right_child;
    } else if (node == node->parent->left) {
      node->parent->left = right_child;
    } else {
      node->parent->right = right_child;
    }
    right_child->left = node;
    node->parent = right_child;
  }

  void RotateRight(TreeNode* node) {
    TreeNode* left_child = node->left;
    node->left = left_child->right;
    if (left_child->right) {
      left_child->right->parent = node;
    }
    left_child->parent = node->parent;
    if (!node->parent) {
      root_ = left_child;
    } else if (node == node->parent->left) {
      node->parent->left = left_child;
    } else {
      node->parent->right = left_child;
    }
    left_child->right = node;
    node->parent = left_child;
  }
  /**
   * @brief Fix the balance of the tree after insertion.
   *
   * @param node
   */
  void FixInsertion(TreeNode* node) {
    while (node != root_ && node->parent->color == RED) {
      if (node->parent == node->parent->parent->left) {
        TreeNode* uncle = node->parent->parent->right;
        if (uncle && uncle->color == RED) {  // Case 1:  uncle node is red
          node->parent->color = BLACK;
          uncle->color = BLACK;
          node->parent->parent->color = RED;
          node = node->parent->parent;
        } else {  // Case 2: uncle node is black or null
          if (node == node->parent->right) {
            node = node->parent;
            RotateLeft(node);  // RotateLeft
          }
          node->parent->color = BLACK;
          node->parent->parent->color = RED;
          RotateRight(node->parent->parent);  // RotateRight
        }
      } else {
        TreeNode* uncle = node->parent->parent->left;
        if (uncle && uncle->color == RED) {  // Case 1: uncle node is red
          node->parent->color = BLACK;
          uncle->color = BLACK;
          node->parent->parent->color = RED;
          node = node->parent->parent;
        } else {  // Case 2: uncle node is black or null
          if (node == node->parent->left) {
            node = node->parent;
            RotateRight(node);  // RotateRight
          }
          node->parent->color = BLACK;
          node->parent->parent->color = RED;
          RotateLeft(node->parent->parent);  // RotateLeft
        }
      }
    }
    root_->color = BLACK;  // the root node is always black.
  }

  void PreOrder(TreeNode* node) const {
    if (node) {
      std::cout << node->data << ", ";
      PreOrder(node->left);
      PreOrder(node->right);
    }
  }

  void InOrder(TreeNode* node) const {
    if (node) {
      InOrder(node->left);
      std::cout << node->data << ", ";
      InOrder(node->right);
    }
  }

  void PostOrder(TreeNode* node) const {
    if (node) {
      PostOrder(node->left);
      PostOrder(node->right);
      std::cout << node->data << ", ";
    }
  }
};

int main() {
  RedBlackTree<int> rb_tree;
  for (int i = 0; i < 10; ++i) {
    rb_tree.Insert(i);
  }
  std::cout << "PreOrder: " << std::endl;
  rb_tree.PreOrder();
  rb_tree.PreOrderIterative();
  std::cout << "InOrder: " << std::endl;
  rb_tree.InOrder();
  rb_tree.InOrderIterative();
  std::cout << "PostOrder: " << std::endl;
  rb_tree.PostOrder();
  rb_tree.PostOrderIterative();
  rb_tree.PostOrderIterativeSingleStack();
  std::cout << "LevelOrder: " << std::endl;
  rb_tree.LevelOrder();
  return 0;
}
