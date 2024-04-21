#include "primer/trie.h"
#include <memory>
#include <stack>
#include <string_view>
#include "common/exception.h"
namespace bustub {
// 1. 根节点为空 返回 nullptr
// 2. 按照key的每一个字符查找，如果遇到找不到的字符返回nullptr
// 3. 遍历结束后的节点如果不是value node 返回nullptr
//     否则返回value
template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  if (!root_) return nullptr;
  auto current_node = root_;
  for (char c : key) {
    if (current_node->children_.find(c) == current_node->children_.end()) return nullptr;
    current_node = current_node->children_.at(c);
  }
  if (!current_node->is_value_node_) return nullptr;
  auto value_node = dynamic_cast<const TrieNodeWithValue<T> *>(current_node.get());
  if (!value_node) return nullptr;
  return value_node->value_.get();
  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
}
// Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
// 1. 按照key查找
// 2. 如果能找到节点就将该节点的children放入栈中
// 3. 如果找不到节点就向栈中放入空的map
// 4. pop一次，创建node_has_value，并将cur_node指向该节点
// 5. 依次{创建一个新节点，pop一次，}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  std::stack<std::shared_ptr<const TrieNode>> s;
  if (!root_) {
    s.push(std::make_shared<const TrieNode>());
  } else {
    s.push(root_);
  }
  for (char c : key) {
    auto query_nod = s.top();
    if (query_nod->children_.find(c) != query_nod->children_.end()) {
      s.push(query_nod->children_.at(c));
    } else {
      s.push(std::make_shared<const TrieNode>());
    }
  }
  // 创建新的value node
  // auto value_node = std::make_shared<const TrieNodeWithValue<T>>();//!
  std::shared_ptr<const TrieNode> cur_node = std::make_shared<const TrieNodeWithValue<T>>(
      std::make_shared<T>(std::move(value)));  // = static_cast<std::shared_ptr<const TrieNode>>(value_node);
  s.pop();
  for (int i = key.size() - 1; i >= 0; i--) {
    char c = key[i];
    auto new_node = s.top()->Clone();
    new_node->children_[c] = cur_node;
    cur_node = std::shared_ptr<const TrieNode>(std::move(new_node));
    s.pop();
  }
  return Trie(cur_node);

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
}
// 1. 按key查找
// 2. 如果找不到 返回原来的trie
// 3. 如果能找到， 将节点入栈
// 4. 先将栈顶pop调，再依次出栈，判断节点是否有值或有大于一个的子节点，
//    如果有则开始重新构建节点
auto Trie::Remove(std::string_view key) const -> Trie {
  std::stack<std::shared_ptr<const TrieNode>> s;
  s.push(root_);
  auto query_node = root_;

  for (char c : key) {
    if (query_node->children_.find(c) != query_node->children_.end()) {
      s.push(query_node->children_.at(c));
    } else {
      return *this;
    }
  }
  s.pop();  // 删除目标节点
  std::shared_ptr<const TrieNode> cur_node(nullptr);
  for (int i = key.length() - 1; i >= 0; i--) {
    char c = key[i];
    // //判段栈顶节点是否有值或 
    //  大于一个子节点
    if (s.empty()) {
      break;
    }
    if (!s.top()->is_value_node_ && (s.top()->children_.size() <= 1)) {
      s.pop();
      continue;
    }
    auto new_node = s.top()->Clone();
    if (cur_node) {
      new_node->children_[c] = cur_node;
    } else {
      new_node->children_.erase(c);
    }
    cur_node = std::shared_ptr<const TrieNode>(std::move(new_node));
    s.pop();
  }
  return Trie(cur_node);

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
