#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#pragma once

/**
 * File contains the following classes:
 *    NodeHandle<Key, Value>
 *    Nodes::BaseNode
 *    Nodes::Node<T>
 *    BaseListIterator<T, bool>
 *    List<T, Alloc>
 *    BaseUMIterator<T, bool>
 *    UnorderedMap<Key, Value, Alloc, Hash, EqualTo>
 */

template <typename Key, typename Value>
struct NodeHandle {
  using key_type = Key;
  using value_type = Value;
  using node_type = std::pair<const Key, Value>;
  size_t hash;
  node_type* pair;
  NodeHandle(size_t& hash, node_type*& pair) : hash(hash), pair(pair) {}
  node_type& get_pair() { return *pair; }
  size_t& get_hash() { return hash; }
};

namespace Nodes {
struct BaseNode {
  BaseNode* prev;
  BaseNode* next;
  BaseNode() : prev(this), next(this) {}
  BaseNode(const BaseNode& other) = default;
  BaseNode& operator=(const BaseNode& other) = default;
  BaseNode(BaseNode&& other) : prev(other.prev), next(other.next) {
    other.prev->next = this;
    other.next->prev = this;
    other.prev = &other;
    other.next = &other;
  }
  BaseNode& operator=(BaseNode&& other) {
    prev = other.prev;
    next = other.next;
    other.prev->next = this;
    other.next->prev = this;
    other.prev = &other;
    other.next = &other;
    return *this;
  }
};

template <typename T>
struct Node : BaseNode {
  T value;
  Node() : BaseNode(), value() {}
  Node(const T& value) : BaseNode(), value(value) {}
  Node(T&& value) : BaseNode(), value(std::move(value)) {}
  Node(const Node<T>& other) : BaseNode(), value(other.value) {}
  Node(Node<T>&& other)
      : BaseNode(std::move(other)), value(std::move(other.value)) {}
  template <typename... Args>
  Node(Args&&... args) : BaseNode(), value(std::forward<Args>(args)...) {}
};
};  // namespace Nodes

/**
================================================================================
                            START  BaseLIstIterator
================================================================================
*/

template <typename T, bool IsConst>
class BaseListIterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = T;
  using reference = typename std::conditional<IsConst, const T&, T&>::type;
  using pointer = typename std::conditional<IsConst, const T*, T*>::type;
  using difference_type = std::ptrdiff_t;

  BaseListIterator(Nodes::BaseNode* node) : node_(node) {}
  BaseListIterator(const BaseListIterator<T, false>& other)
      : node_(other.get_node()) {}
  ~BaseListIterator() {}

  BaseListIterator& operator++() {
    node_ = node_->next;
    return *this;
  }
  BaseListIterator operator++(int) {
    BaseListIterator<T, IsConst> tmp = *this;
    ++*this;
    return tmp;
  }
  BaseListIterator& operator--() {
    node_ = node_->prev;
    return *this;
  }
  BaseListIterator operator--(int) {
    BaseListIterator<T, IsConst> tmp = *this;
    --*this;
    return tmp;
  }

  template <typename U, bool IsConst1, bool IsConst2>
  friend bool operator==(const BaseListIterator<U, IsConst1>&,
                         const BaseListIterator<U, IsConst2>&);

  reference operator*() {
    return reinterpret_cast<Nodes::Node<T>*>(node_)->value;
  }
  pointer operator->() {
    return &reinterpret_cast<Nodes::Node<T>*>(node_)->value;
  }

  Nodes::BaseNode* get_node() const { return node_; }

 private:
  Nodes::BaseNode* node_;
};

template <typename U, bool IsConst1, bool IsConst2>
bool operator==(const BaseListIterator<U, IsConst1>& it1,
                const BaseListIterator<U, IsConst2>& it2) {
  return it1.node_ == it2.node_;
}

template <typename U, bool IsConst1, bool IsConst2>
bool operator!=(const BaseListIterator<U, IsConst1>& it1,
                const BaseListIterator<U, IsConst2>& it2) {
  return !(it1 == it2);
}

/**
================================================================================
                            END  BaseLIstIterator
================================================================================
*/

/**
================================================================================
                                START  List
================================================================================
*/

template <typename T, typename Alloc = std::allocator<T>>
class List {
 public:
  using value_type = T;
  using size_type = size_t;

  using AllocTraits = std::allocator_traits<Alloc>;
  using NodeAlloc = typename AllocTraits::template rebind_alloc<Nodes::Node<T>>;
  using NodeAllocTraits =
      typename AllocTraits::template rebind_traits<Nodes::Node<T>>;

  using iterator = BaseListIterator<T, false>;
  using const_iterator = BaseListIterator<T, true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 private:
  template <typename, bool>
  friend class BaseListIterator;

 public:
  List();
  List(size_type);
  List(size_type, const T&);
  List(Alloc&);
  List(size_type, Alloc&);
  List(size_type, const T&, Alloc&);

  List(const List&);
  List(List&&);
  List& operator=(const List&);
  List& operator=(List&&);
  ~List();

  NodeAlloc get_allocator() const;
  size_type size() const;
  void relink_node(Nodes::BaseNode*, Nodes::BaseNode*);

  template <typename... Args>
  void emplace(const_iterator, Args&&...);
  void insert(const_iterator, const T&);
  void insert(const_iterator, T&&);
  void erase(const_iterator);

  template <typename... Args>
  void emplace_back(Args&&...);
  template <typename... Args>
  void emplace_front(Args&&...);

  void push_front(const T&);
  void push_front(T&&);
  void push_back(const T&);
  void push_back(T&&);
  void pop_front();
  void pop_back();

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;

  reverse_iterator rbegin();
  reverse_iterator rend();
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

 private:
  void fill_by_default_value(size_type);
  void fill_by_value(size_type, const T&);
  void fill_by_other_list(const_iterator, const_iterator, size_type);
  void link_before_node(Nodes::BaseNode*, Nodes::BaseNode*);
  void unlink_node(Nodes::BaseNode*);
  void destroy_nodes();

  NodeAlloc node_alloc_;
  size_type sz_;
  Nodes::BaseNode base_node_;
};

template <typename T, typename Alloc>
void List<T, Alloc>::fill_by_default_value(size_type sz) {
  try {
    for (size_type i = 0; i < sz; ++i) {
      emplace_back();
    }
  } catch (...) {
    destroy_nodes();
    throw;
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::fill_by_value(size_type sz, const T& t) {
  try {
    for (size_type i = 0; i < sz; ++i) {
      emplace_back(t);
    }
  } catch (...) {
    destroy_nodes();
    throw;
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::fill_by_other_list(const_iterator begin,
                                        const_iterator end,
                                        size_type other_sz) {
  size_type counter = 0;
  try {
    for (auto it = begin; it != end; ++counter, ++it) {
      emplace_back(*it);
    }
    for (; sz_ > other_sz;) {
      erase(this->begin());
    }
  } catch (...) {
    for (; counter > 0; --counter) {
      erase(--this->end());
    }
    throw;
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::destroy_nodes() {
  for (; begin() != end();) {
    erase(begin());
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::link_before_node(Nodes::BaseNode* new_node,
                                      Nodes::BaseNode* before_node_) {
  new_node->prev = before_node_->prev;
  new_node->next = before_node_;

  new_node->prev->next = new_node;
  new_node->next->prev = new_node;
}

template <typename T, typename Alloc>
void List<T, Alloc>::unlink_node(Nodes::BaseNode* node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;
}

template <typename T, typename Alloc>
void List<T, Alloc>::relink_node(Nodes::BaseNode* from,
                                 Nodes::BaseNode* before_to) {
  unlink_node(from);
  link_before_node(from, before_to);
}

template <typename T, typename Alloc>
List<T, Alloc>::List() : node_alloc_(), sz_(0), base_node_() {}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type sz) : node_alloc_(), sz_(0), base_node_() {
  fill_by_default_value(sz);
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type sz, const T& t)
    : node_alloc_(), sz_(0), base_node_() {
  fill_by_value(sz, t);
}

template <typename T, typename Alloc>
List<T, Alloc>::List(Alloc& alloc) : node_alloc_(alloc), sz_(0), base_node_() {}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type sz, Alloc& alloc)
    : node_alloc_(alloc), sz_(0), base_node_() {
  fill_by_default_value(sz);
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type sz, const T& t, Alloc& alloc)
    : node_alloc_(alloc), sz_(0), base_node_() {
  fill_by_value(sz, t);
}

template <typename T, typename Alloc>
List<T, Alloc>::List(const List& other)
    : node_alloc_(NodeAllocTraits::select_on_container_copy_construction(
          other.node_alloc_)),
      sz_(0),
      base_node_() {
  fill_by_other_list(other.cbegin(), other.cend(), other.sz_);
}

template <typename T, typename Alloc>
List<T, Alloc>::List(List&& other)
    : node_alloc_(std::move(other.node_alloc_)),
      sz_(other.sz_),
      base_node_(std::move(other.base_node_)) {
  other.sz_ = 0;
}

template <typename T, typename Alloc>
List<T, Alloc>& List<T, Alloc>::operator=(const List& other) {
  if (NodeAllocTraits::propagate_on_container_copy_assignment::value) {
    node_alloc_ = other.node_alloc_;
  } else {
    node_alloc_ = NodeAllocTraits::select_on_container_copy_construction(
        other.node_alloc_);
  }
  fill_by_other_list(other.cbegin(), other.cend(), other.sz_);
  return *this;
}

template <typename T, typename Alloc>
List<T, Alloc>& List<T, Alloc>::operator=(List&& other) {
  if (NodeAllocTraits::propagate_on_container_move_assignment::value) {
    node_alloc_ = other.node_alloc_;
  } else {
    node_alloc_ = NodeAllocTraits::select_on_container_copy_construction(
        other.node_alloc_);
  }
  destroy_nodes();
  base_node_ = std::move(other.base_node_);
  sz_ = other.sz_;
  other.sz_ = 0;
  return *this;
}

template <typename T, typename Alloc>
List<T, Alloc>::~List() {
  destroy_nodes();
}

template <typename T, typename Alloc>
typename List<T, Alloc>::size_type List<T, Alloc>::size() const {
  return sz_;
}

template <typename T, typename Alloc>
typename List<T, Alloc>::NodeAlloc List<T, Alloc>::get_allocator() const {
  return node_alloc_;
}

template <typename T, typename Alloc>
template <typename... Args>
void List<T, Alloc>::emplace(const_iterator it, Args&&... args) {
  Nodes::Node<T>* new_node = NodeAllocTraits::allocate(node_alloc_, 1);
  try {
    NodeAllocTraits::construct(node_alloc_, new_node,
                               std::forward<Args>(args)...);
  } catch (...) {
    NodeAllocTraits::deallocate(node_alloc_, new_node, 1);
    throw;
  }

  link_before_node(new_node, it.get_node());
  ++sz_;
}

template <typename T, typename Alloc>
template <typename... Args>
void List<T, Alloc>::emplace_back(Args&&... args) {
  emplace(cend(), std::forward<Args>(args)...);
}

template <typename T, typename Alloc>
template <typename... Args>
void List<T, Alloc>::emplace_front(Args&&... args) {
  emplace(cbegin(), std::forward<Args>(args)...);
}

template <typename T, typename Alloc>
void List<T, Alloc>::insert(const_iterator it, T&& t) {
  emplace(it, std::move(t));
}

template <typename T, typename Alloc>
void List<T, Alloc>::insert(const_iterator it, const T& t) {
  emplace(it, t);
}

template <typename T, typename Alloc>
void List<T, Alloc>::erase(const_iterator it) {
  unlink_node(it.get_node());
  NodeAllocTraits::destroy(node_alloc_,
                           reinterpret_cast<Nodes::Node<T>*>(it.get_node()));
  NodeAllocTraits::deallocate(
      node_alloc_, reinterpret_cast<Nodes::Node<T>*>(it.get_node()), 1);
  --sz_;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_front(T&& t) {
  emplace(cbegin(), std::move(t));
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back(T&& t) {
  emplace(cend(), std::move(t));
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_front(const T& t) {
  emplace(cbegin(), t);
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back(const T& t) {
  emplace(cend(), t);
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_front() {
  erase(cbegin());
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_back() {
  erase(--cend());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::begin() {
  return iterator(base_node_.next);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::end() {
  return iterator(&base_node_);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::begin() const {
  return const_iterator(base_node_.next);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::end() const {
  return const_iterator(const_cast<Nodes::BaseNode*>(&base_node_));
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::cbegin() const {
  return const_iterator(base_node_.next);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::cend() const {
  return const_iterator(const_cast<Nodes::BaseNode*>(&base_node_));
}

template <typename T, typename Alloc>
typename List<T, Alloc>::reverse_iterator List<T, Alloc>::rbegin() {
  return std::reverse_iterator(end());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::reverse_iterator List<T, Alloc>::rend() {
  return std::reverse_iterator(begin());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::rbegin() const {
  return std::reverse_iterator(cend());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::rend() const {
  return std::reverse_iterator(cbegin());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::crbegin()
    const {
  return std::reverse_iterator(cend());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::crend() const {
  return std::reverse_iterator(cbegin());
}

/**
================================================================================
                                END  List
================================================================================
*/

/**
================================================================================
                            START BaseUMIterator
================================================================================
*/

template <typename T, bool IsConst>
class BaseUMIterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename T::node_type;
  using reference =
      typename std::conditional<IsConst, const value_type&, value_type&>::type;
  using pointer =
      typename std::conditional<IsConst, const value_type*, value_type*>::type;
  using difference_type = std::ptrdiff_t;

  BaseUMIterator(Nodes::BaseNode* b_node) : list_it_(b_node) {}

  BaseUMIterator(const BaseUMIterator<T, false>& other)
      : list_it_(other.get_node()) {}
  BaseUMIterator& operator=(const BaseUMIterator& other) = default;
  ~BaseUMIterator() {}

  BaseUMIterator& operator++() {
    ++list_it_;
    return *this;
  }
  BaseUMIterator operator++(int) {
    auto tmp(*this);
    ++list_it_;
    return tmp;
  }
  BaseUMIterator& operator--() {
    --list_it_;
    return *this;
  }
  BaseUMIterator operator--(int) {
    auto tmp(*this);
    --list_it_;
    return tmp;
  }

  template <typename U, bool IsConst1, bool IsConst2>
  friend bool operator==(const BaseUMIterator<U, IsConst1>&,
                         const BaseUMIterator<U, IsConst2>&);

  reference operator*() { return *(list_it_->pair); }
  pointer operator->() { return list_it_->pair; }

  Nodes::BaseNode* get_node() const { return list_it_.get_node(); }
  size_t get_hash() { return list_it_->hash; }

 private:
  BaseListIterator<T, IsConst> list_it_;
};

template <typename U, bool IsConst1, bool IsConst2>
bool operator==(const BaseUMIterator<U, IsConst1>& it1,
                const BaseUMIterator<U, IsConst2>& it2) {
  return it1.list_it_ == it2.list_it_;
}

template <typename U, bool IsConst1, bool IsConst2>
bool operator!=(const BaseUMIterator<U, IsConst1>& it1,
                const BaseUMIterator<U, IsConst2>& it2) {
  return !(it1 == it2);
}

/**
================================================================================
                            END BaseUMIterator
================================================================================
*/

/**
================================================================================
                            START UnorderedMap
================================================================================
*/

template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename EqualTo = std::equal_to<Key>,
          typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
 public:
  using NodeType = std::pair<const Key, Value>;
  using node_handle = NodeHandle<Key, Value>;

  using iterator = BaseUMIterator<NodeHandle<Key, Value>, false>;
  using const_iterator = BaseUMIterator<NodeHandle<Key, Value>, true>;

  using NodeAllocTraits = std::allocator_traits<Alloc>;
  using BucketAlloc =
      typename NodeAllocTraits::template rebind_alloc<Nodes::BaseNode*>;

  using array_type = std::vector<Nodes::BaseNode*, BucketAlloc>;
  using list_type = List<NodeHandle<Key, Value>, Alloc>;

 private:
  static constexpr size_t default_count_ = 64;
  static constexpr float default_max_load_factor_ = 0.5;
  Alloc node_alloc_;
  array_type bucket_count_;
  list_type list_;
  float max_load_factor_;

 public:
  UnorderedMap()
      : node_alloc_(),
        bucket_count_(default_count_, nullptr),
        list_(),
        max_load_factor_(default_max_load_factor_) {}
  UnorderedMap(const UnorderedMap& other)
      : node_alloc_(NodeAllocTraits::select_on_container_copy_construction(
            other.node_alloc_)),
        bucket_count_(other.bucket_count(), nullptr),
        list_(),
        max_load_factor_(other.max_load_factor_) {
    for (auto it = other.begin(); it != other.end(); ++it) {
      NodeType* node = NodeAllocTraits::allocate(node_alloc_, 1);
      NodeAllocTraits::construct(node_alloc_, node, *it);
      size_t hash = it.get_hash();
      list_.emplace_back(hash, node);
    }
    if (size() != 0) {
      bucket_count_[begin().get_hash() % bucket_count()] = begin().get_node();
      for (auto fst = begin(), snd = ++begin(); snd != end(); ++fst, ++snd) {
        if (fst.get_hash() % bucket_count() !=
            snd.get_hash() % bucket_count()) {
          bucket_count_[snd.get_hash() % bucket_count()] = snd.get_node();
        }
      }
    }
  }
  UnorderedMap(UnorderedMap&& other)
      : node_alloc_(std::move(other.node_alloc_)),
        bucket_count_(std::move(other.bucket_count_)),
        list_(std::move(other.list_)),
        max_load_factor_(other.max_load_factor_) {
    other.bucket_count_ = array_type(default_count_, nullptr);
    other.max_load_factor_ = default_max_load_factor_;
  }

  UnorderedMap& operator=(const UnorderedMap& other) {
    if (NodeAllocTraits::propagate_on_container_copy_assignment::value) {
      node_alloc_ = other.node_alloc_;
    } else {
      node_alloc_ = NodeAllocTraits::select_on_container_copy_construction(
          other.node_alloc_);
    }

    list_type tmp(other.list_);
    CleanNodes();
    list_ = std::move(tmp);
    bucket_count_ = array_type(other.bucket_count(), nullptr);
    max_load_factor_ = other.max_load_factor_;

    if (size() != 0) {
      bucket_count_[begin().get_hash() % bucket_count()] = begin().get_node();
      for (auto fst = begin(), snd = ++begin(); snd != end(); ++fst, ++snd) {
        if (fst.get_hash() % bucket_count() !=
            snd.get_hash() % bucket_count()) {
          bucket_count_[snd.get_hash() % bucket_count()] = snd.get_node();
        }
      }
    }
    return *this;
  }
  UnorderedMap& operator=(UnorderedMap&& other) {
    if (NodeAllocTraits::propagate_on_container_move_assignment::value) {
      node_alloc_ = other.node_alloc_;
    } else {
      node_alloc_ = NodeAllocTraits::select_on_container_copy_construction(
          other.node_alloc_);
    }
    CleanNodes();
    list_ = std::move(other.list_);
    bucket_count_ = std::move(other.bucket_count_);
    max_load_factor_ = other.max_load_factor_;

    other.bucket_count_ = array_type(default_count_, nullptr);
    other.max_load_factor_ = default_max_load_factor_;
    return *this;
  }
  void swap(UnorderedMap& other) {
    if (NodeAllocTraits::propagate_on_container_swap::value) {
      node_alloc_.swap(other.node_alloc_);
    } else {
      std::swap(node_alloc_, other.node_alloc_);
    }
    std::swap(list_, other.list_);
    std::swap(bucket_count_, other.bucket_count_);
    std::swap(max_load_factor_, other.max_load_factor_);
  }
  ~UnorderedMap() { CleanNodes(); }

  void CleanNodes() {
    for (auto it = begin(); it != end(); ++it) {
      NodeAllocTraits::destroy(node_alloc_, &(*it));
      NodeAllocTraits::deallocate(node_alloc_, &(*it), 1);
    }
  }

  Value& operator[](const Key& key) {
    auto res = insert({key, Value()});
    return res.first->second;
  }
  Value& operator[](Key&& key) {
    auto res = emplace(std::move(key), std::move(Value()));
    return res.first->second;
  }
  Value& at(const Key& key) {
    auto it = find(key);
    if (it == end()) {
      throw std::out_of_range("index out of range\n");
    }
    return it->second;
  }
  const Value& at(const Key& key) const {
    auto cit = find(key);
    if (cit == cend()) {
      throw std::out_of_range("index out of range\n");
    }
    return cit->second;
  }

  size_t size() const { return list_.size(); }

  iterator begin() { return iterator(list_.begin().get_node()); }
  iterator end() { return iterator(list_.end().get_node()); }
  const_iterator begin() const {
    return const_iterator(list_.begin().get_node());
  }
  const_iterator end() const { return const_iterator(list_.end().get_node()); }
  const_iterator cbegin() const {
    return const_iterator(list_.begin().get_node());
  }
  const_iterator cend() const { return const_iterator(list_.end().get_node()); }

  std::pair<iterator, bool> insert(const NodeType& node) {
    return emplace(node);
  }
  std::pair<iterator, bool> insert(NodeType&& node) {
    return emplace(std::move(node));
  }

  template <typename P>
  std::pair<iterator, bool> insert(P&& node) {
    return emplace(std::move(node));
  }
  template <typename InputIt>
  void insert(InputIt it1, InputIt it2) {
    for (; it1 != it2; ++it1) {
      emplace(*it1);
    }
  }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    std::pair<iterator, bool> res = {end(), false};
    NodeType* node = NodeAllocTraits::allocate(node_alloc_, 1);
    try {
      NodeAllocTraits::construct(node_alloc_, node,
                                 std::forward<Args>(args)...);
    } catch (...) {
      NodeAllocTraits::deallocate(node_alloc_, node, 1);
      throw;
    }
    size_t hash = Hash()(node->first);
    size_t idx = hash % bucket_count();
    if (bucket_count_[idx] == nullptr) {
      list_.emplace_front(hash, node);
      bucket_count_[idx] = list_.begin().get_node();
      res = {begin(), true};
    } else {
      try {
        iterator it(bucket_count_[idx]);
        bool break_out = false;
        for (; it != end() && it.get_hash() % bucket_count() == idx; ++it) {
          if (EqualTo()(it->first, node->first)) {
            NodeAllocTraits::destroy(node_alloc_, node);
            NodeAllocTraits::deallocate(node_alloc_, node, 1);
            res = {it, false};
            break_out = true;
            break;
          }
        }
        if (!break_out) {
          list_.emplace(it.get_node(), hash, node);
          res = {--it, true};
        }
      } catch (...) {
        throw;
      }
    }

    if (load_factor() > max_load_factor()) {
      rehash(2 * bucket_count());
    }

    return res;
  }

  void erase(const_iterator it) {
    auto node = const_cast<NodeType*>(&(*it));
    NodeAllocTraits::destroy(node_alloc_, node);
    NodeAllocTraits::deallocate(node_alloc_, node, 1);
    size_t idx = it.get_hash() % bucket_count();
    if (it.get_node() == bucket_count_[idx]) {
      iterator next(it.get_node()->next);
      if (next == end() || next.get_hash() % bucket_count() != idx) {
        bucket_count_[idx] = nullptr;
      } else {
        bucket_count_[idx] = next.get_node();
      }
    }
    list_.erase(it.get_node());
  }
  void erase(const_iterator it1, const_iterator it2) {
    for (; it1 != it2;) {
      erase(it1++);
    }
  }

  iterator find(const Key& key) {
    size_t hash = Hash()(key);
    size_t idx = hash % bucket_count();
    if (bucket_count_[idx] == nullptr) {
      return end();
    }
    auto it = iterator(bucket_count_[idx]);
    for (; it != end() && it.get_hash() % bucket_count() == idx; ++it) {
      if (EqualTo()(it->first, key)) {
        return it;
      }
    }
    return end();
  }
  const_iterator find(const Key& key) const {
    size_t hash = Hash()(key);
    size_t idx = hash % bucket_count();
    if (bucket_count_[idx] == nullptr) {
      return cend();
    }
    auto it = const_iterator(bucket_count_[idx]);
    for (; it != end() && it.get_hash() % bucket_count() == idx; ++it) {
      if (it.get_hash() == hash) {
        return it;
      }
    }
    return cend();
  }

  void reserve(size_t count) {
    count = std::max(count, (size_t)std::ceil(size() / max_load_factor_));
    if (count > bucket_count_.size()) {
      rehash(count);
    }
  }
  void rehash(size_t count) {
    bucket_count_ = array_type(count);

    for (auto it = begin(); it != end();) {
      size_t hash = it.get_hash();
      size_t idx = hash % bucket_count();
      if (bucket_count_[idx] == nullptr) {
        bucket_count_[idx] = it.get_node();
      } else {
        iterator next(bucket_count_[idx]->next);
        list_.relink_node(it.get_node(), next.get_node());
      }
      ++it;
    }
  }
  float load_factor() const { return size() / bucket_count(); }
  void max_load_factor(float mlf) { max_load_factor_ = mlf; }
  float max_load_factor() const { return max_load_factor_; }
  size_t bucket_count() const { return bucket_count_.size(); }

  void PrintBucketCount() {
    std::cout << "\nstart backet_count\n";
    for (size_t i = 0; i < std::min((size_t)10, bucket_count()); ++i) {
      std::cout << bucket_count_[i] << "\n";
    }
    if (bucket_count() > 10) {
      std::cout << "...\n";
    }
    std::cout << "end backet_count\n\n";
  }

  void PrintList() {
    std::cout << "\nstart list\n";
    auto it = begin();
    for (size_t i = 0; i < std::min((size_t)10, size()); ++i, ++it) {
      std::cout << it.get_node()->prev << "\t" << it.get_node() << "\t"
                << it.get_node()->next << "\t" << it.get_hash() << "\n";
    }
    if (size() > 10) {
      std::cout << "...\n";
    }
    std::cout << "end list\n\n";
  }
};
