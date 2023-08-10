#include <array>
#include <iostream>
#include <memory>

/**
================================================================================
                              DECLARATION
================================================================================
*/

template <size_t N>
class StackStorage {
   public:
  StackStorage() = default;
  ~StackStorage() = default;
  StackStorage(const StackStorage&) = delete;
  template <typename T>
  T* allocate(size_t size);

   private:
  size_t shift = 0;
  std::array<char, N> data_;
};

template <typename T, size_t N>
class StackAllocator {
   public:
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using const_pointer = const T*;
  using const_reference = const T&;
  using size_type = size_t;

  StackAllocator();
  StackAllocator(StackStorage<N>&);
  ~StackAllocator();
  template <typename U>
  StackAllocator(const StackAllocator<U, N>&);
  StackAllocator& operator=(const StackAllocator&);

  pointer allocate(size_type n);
  void deallocate(pointer p, size_type n);

  StackStorage<N>* get_storage() const;

  template <typename U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };

   private:
  StackStorage<N>* stack_storage_;
};

namespace Nodes {
struct BaseNode {
  BaseNode* prev;
  BaseNode* next;
  BaseNode() : prev(this), next(this) {}
};

template <typename T>
struct Node : BaseNode {
  T val;
  Node() : BaseNode(), val() {}
  Node(const T& val) : BaseNode(), val(val) {}
  Node(const Node<T>& other) : BaseNode(), val(other.val) {}
};
};  // namespace Nodes

template <typename T, bool IsConst>
class BaseListIterator {
   public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = T;
  using reference = typename std::conditional<IsConst, const T&, T&>::type;
  using pointer = typename std::conditional<IsConst, const T*, T*>::type;
  using difference_type = std::ptrdiff_t;

  BaseListIterator(Nodes::BaseNode*);
  BaseListIterator(const BaseListIterator<T, false>&);
  ~BaseListIterator();

  BaseListIterator& operator++();
  BaseListIterator operator++(int);
  BaseListIterator& operator--();
  BaseListIterator operator--(int);

  template <typename U, bool IsConst1, bool IsConst2>
  friend bool operator==(const BaseListIterator<U, IsConst1>&,
                         const BaseListIterator<U, IsConst2>&);

  reference operator*();
  pointer operator->();

  Nodes::BaseNode* get_node() const;
  BaseListIterator<T, false> remove_const();

   private:
  Nodes::BaseNode* node_;
};

template <typename T, typename Alloc = std::allocator<T>>
class List {
   public:
  using value_type = T;
  using size_type = size_t;

  using AllocTraits = std::allocator_traits<Alloc>;
  using NodeAlloc = typename AllocTraits::template rebind_alloc<Nodes::Node<T>>;
  using NodeAllocTraits = std::allocator_traits<NodeAlloc>;

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

  NodeAlloc get_allocator() const;

  List(const List&);
  List& operator=(const List&);
  ~List();

  size_type size() const;

  void push_front(const T&);
  void push_back(const T&);
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

  void insert(const_iterator, const T&);

  void erase(const_iterator);

   private:
  void fill_by_default_value();
  void fill_by_value(const T&);
  void fill_by_other_list(const_iterator, const_iterator, size_type);
  void link_before_node(Nodes::BaseNode*, Nodes::BaseNode*);
  void destroy_nodes();

  NodeAlloc node_alloc_;
  size_type sz_;
  Nodes::BaseNode base_node_;
};

/**
================================================================================
                            List DEFINITION
================================================================================
*/

template <typename T, typename Alloc>
inline void List<T, Alloc>::fill_by_default_value() {
  Nodes::Node<T>* new_node;
  try {
    for (size_type i = 0; i < sz_; ++i) {
      new_node = NodeAllocTraits::allocate(node_alloc_, 1);
      NodeAllocTraits::construct(node_alloc_, new_node);
      link_before_node(new_node, &base_node_);
    }
  } catch (...) {
    NodeAllocTraits::deallocate(node_alloc_, new_node, 1);
    destroy_nodes();
    throw;
  }
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::fill_by_value(const T& t) {
  Nodes::Node<T>* new_node;
  try {
    for (size_type i = 0; i < sz_; ++i) {
      new_node = NodeAllocTraits::allocate(node_alloc_, 1);
      NodeAllocTraits::construct(node_alloc_, new_node, t);
      link_before_node(new_node, base_node_);
    }
  } catch (...) {
    NodeAllocTraits::deallocate(node_alloc_, new_node, 1);
    destroy_nodes();
    throw;
  }
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::fill_by_other_list(const_iterator begin,
                                               const_iterator end,
                                               size_type other_sz) {
  size_type counter = 0;
  Nodes::Node<T>* new_node;
  try {
    for (auto it = begin; it != end; ++counter, ++it) {
      new_node = NodeAllocTraits::allocate(node_alloc_, 1);
      NodeAllocTraits::construct(node_alloc_, new_node, *it);
      link_before_node(new_node, this->end().get_node());
      ++sz_;
    }

    for (; sz_ > other_sz;) {
      erase(this->begin());
    }

    sz_ = other_sz;
  } catch (...) {
    NodeAllocTraits::deallocate(node_alloc_, new_node, 1);
    for (; counter > 0; --counter) {
      erase(--this->end());
    }
    throw;
  }
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::destroy_nodes() {
  for (; begin() != end();) {
    erase(begin());
  }
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::link_before_node(Nodes::BaseNode* new_node,
                                             Nodes::BaseNode* before_node_) {
  new_node->prev = before_node_->prev;
  new_node->next = before_node_;

  new_node->prev->next = new_node;
  new_node->next->prev = new_node;
}

template <typename T, typename Alloc>
List<T, Alloc>::List() : node_alloc_(), sz_(0), base_node_() {}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type sz) : node_alloc_(), sz_(sz), base_node_() {
  fill_by_default_value();
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type sz, const T& t)
    : node_alloc_(), sz_(sz), base_node_() {
  fill_by_value(t);
}

template <typename T, typename Alloc>
List<T, Alloc>::List(Alloc& alloc) : node_alloc_(alloc), sz_(0), base_node_() {}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type sz, Alloc& alloc)
    : node_alloc_(alloc), sz_(sz), base_node_() {
  fill_by_default_value();
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_type sz, const T& t, Alloc& alloc)
    : node_alloc_(alloc), sz_(sz), base_node_() {
  fill_by_value(t);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::NodeAlloc List<T, Alloc>::get_allocator() const {
  return node_alloc_;
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
List<T, Alloc>::~List() {
  destroy_nodes();
}

template <typename T, typename Alloc>
typename List<T, Alloc>::size_type List<T, Alloc>::size() const {
  return sz_;
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::push_front(const T& t) {
  insert(cbegin(), t);
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::push_back(const T& t) {
  insert(cend(), t);
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::pop_front() {
  erase(cbegin());
}

template <typename T, typename Alloc>
inline void List<T, Alloc>::pop_back() {
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

template <typename T, typename Alloc>
void List<T, Alloc>::insert(const_iterator it, const T& t) {
  Nodes::Node<T>* new_node = NodeAllocTraits::allocate(node_alloc_, 1);
  try {
    NodeAllocTraits::construct(node_alloc_, new_node, t);
  } catch (...) {
    NodeAllocTraits::deallocate(node_alloc_, new_node, 1);
    throw;
  }

  link_before_node(new_node, it.get_node());
  ++sz_;
}

template <typename T, typename Alloc>
void List<T, Alloc>::erase(const_iterator it) {
  it.get_node()->prev->next = it.get_node()->next;
  it.get_node()->next->prev = it.get_node()->prev;
  NodeAllocTraits::destroy(node_alloc_,
                           reinterpret_cast<Nodes::Node<T>*>(it.get_node()));
  NodeAllocTraits::deallocate(
      node_alloc_, reinterpret_cast<Nodes::Node<T>*>(it.get_node()), 1);
  --sz_;
}

/**
================================================================================
                        BaseListIterator DEFINITION
================================================================================
*/

template <typename T, bool IsConst>
BaseListIterator<T, IsConst>::BaseListIterator(Nodes::BaseNode* node)
    : node_(node) {}

template <typename T, bool IsConst>
BaseListIterator<T, IsConst>::BaseListIterator(
    const BaseListIterator<T, false>& other)
    : node_(other.get_node()) {}

template <typename T, bool IsConst>
BaseListIterator<T, IsConst>::~BaseListIterator() {}

template <typename T, bool IsConst>
BaseListIterator<T, IsConst>& BaseListIterator<T, IsConst>::operator++() {
  node_ = node_->next;
  return *this;
}

template <typename T, bool IsConst>
BaseListIterator<T, IsConst> BaseListIterator<T, IsConst>::operator++(int) {
  BaseListIterator<T, IsConst> tmp = *this;
  ++*this;
  return tmp;
}

template <typename T, bool IsConst>
BaseListIterator<T, IsConst>& BaseListIterator<T, IsConst>::operator--() {
  node_ = node_->prev;
  return *this;
}

template <typename T, bool IsConst>
BaseListIterator<T, IsConst> BaseListIterator<T, IsConst>::operator--(int) {
  BaseListIterator<T, IsConst> tmp = *this;
  --*this;
  return tmp;
}

template <typename T, bool IsConst>
typename BaseListIterator<T, IsConst>::reference
BaseListIterator<T, IsConst>::operator*() {
  return reinterpret_cast<Nodes::Node<T>*>(node_)->val;
}

template <typename T, bool IsConst>
typename BaseListIterator<T, IsConst>::pointer
BaseListIterator<T, IsConst>::operator->() {
  return &reinterpret_cast<Nodes::Node<T>*>(node_)->val;
}

template <typename T, bool IsConst>
Nodes::BaseNode* BaseListIterator<T, IsConst>::get_node() const {
  return node_;
}

template <typename T, bool IsConst>
inline BaseListIterator<T, false> BaseListIterator<T, IsConst>::remove_const() {
  return BaseListIterator<T, false>(node_);
}

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
                           StackStorage DEFINITION
================================================================================
*/

template <size_t N>
template <typename T>
T* StackStorage<N>::allocate(size_t size) {
  size_t space = N - shift;
  void* ptr = data_.data() + shift;
  if (std::align(alignof(T), sizeof(T), ptr, space)) {
    T* result = reinterpret_cast<T*>(ptr);
    shift += (N - shift) - space;
    shift += sizeof(T) * size;
    return result;
  }
  return nullptr;
}

/**
================================================================================
                          StackAllocator DEFINITION
================================================================================
*/

template <typename T, size_t N>
StackAllocator<T, N>::StackAllocator(StackStorage<N>& stack_storage)
    : stack_storage_(&stack_storage) {}

template <typename T, size_t N>
inline StackAllocator<T, N>::~StackAllocator() {}

template <typename T, size_t N>
template <typename U>
StackAllocator<T, N>::StackAllocator(const StackAllocator<U, N>& other)
    : stack_storage_(other.get_storage()) {}

template <typename T, size_t N>
inline StackAllocator<T, N>& StackAllocator<T, N>::operator=(
    const StackAllocator& other) {
  stack_storage_ = other.stack_storage_;
  return *this;
}

template <typename T, size_t N>
typename StackAllocator<T, N>::pointer StackAllocator<T, N>::allocate(
    size_type n) {
  return (*stack_storage_).template allocate<T>(n);
}

template <typename T, size_t N>
void StackAllocator<T, N>::deallocate(pointer p, size_type n) {
  std::ignore = p;
  std::ignore = n;
}

template <typename T, size_t N>
StackStorage<N>* StackAllocator<T, N>::get_storage() const {
  return stack_storage_;
}
