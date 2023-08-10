#include <iostream>

//=========================================================================================
//                                    DECLARATION
//=========================================================================================

template <typename T>
class Deque;

template <typename T, bool IsConst>
class BaseDequeIterator {
 public:
  using value_type = T;
  using iterator_category = std::random_access_iterator_tag;
  using reference = typename std::conditional<IsConst, const T&, T&>::type;
  using pointer = typename std::conditional<IsConst, const T*, T*>::type;
  using size_type = typename Deque<T>::size_type;
  using difference_type = typename Deque<T>::difference_type;

  BaseDequeIterator(size_type pos_out_basket, size_type pos_in_basket,
                    const Deque<T>* deque);

  BaseDequeIterator(const BaseDequeIterator<T, false>& it);
  ~BaseDequeIterator() = default;

  BaseDequeIterator& operator++();
  BaseDequeIterator operator++(int);
  BaseDequeIterator& operator--();
  BaseDequeIterator operator--(int);
  BaseDequeIterator& operator+=(difference_type diff);
  BaseDequeIterator& operator-=(difference_type diff);

  BaseDequeIterator<T, false> remove_const();

  template <typename U, bool IsConst1, bool IsConst2>
  friend bool operator<(const BaseDequeIterator<U, IsConst1>& it1,
                        const BaseDequeIterator<U, IsConst2>& it2);

  template <typename U, bool IsConst1, bool IsConst2>
  friend bool operator==(const BaseDequeIterator<U, IsConst1>& it1,
                         const BaseDequeIterator<U, IsConst2>& it2);

  template <typename U, bool IsConst1, bool IsConst2>
  friend std::ptrdiff_t operator-(BaseDequeIterator<U, IsConst1> it1,
                                  BaseDequeIterator<U, IsConst2> it2);

  reference operator*() const;
  pointer operator->() const;

 private:
  static const size_type basket_size_ = Deque<T>::basket_size_;

  size_type pos_out_basket_;
  size_type pos_in_basket_;
  const Deque<T>* deque_;
};

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst> operator+(
    BaseDequeIterator<T, IsConst> it,
    typename BaseDequeIterator<T, IsConst>::difference_type diff);

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst> operator+(
    typename BaseDequeIterator<T, IsConst>::difference_type diff,
    BaseDequeIterator<T, IsConst> it);

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst> operator-(
    BaseDequeIterator<T, IsConst> it,
    typename BaseDequeIterator<T, IsConst>::difference_type diff);

template <typename T, bool IsConst1, bool IsConst2>
std::ptrdiff_t operator-(BaseDequeIterator<T, IsConst1> it1,
                         BaseDequeIterator<T, IsConst2> it2);

template <typename T, bool IsConst1, bool IsConst2>
bool operator<(const BaseDequeIterator<T, IsConst1>& it1,
               const BaseDequeIterator<T, IsConst2>& it2);

template <typename T, bool IsConst1, bool IsConst2>
bool operator>(const BaseDequeIterator<T, IsConst1>& it1,
               const BaseDequeIterator<T, IsConst2>& it2);

template <typename T, bool IsConst1, bool IsConst2>
bool operator<=(const BaseDequeIterator<T, IsConst1>& it1,
                const BaseDequeIterator<T, IsConst2>& it2);

template <typename T, bool IsConst1, bool IsConst2>
bool operator>=(const BaseDequeIterator<T, IsConst1>& it1,
                const BaseDequeIterator<T, IsConst2>& it2);

template <typename T, bool IsConst1, bool IsConst2>
bool operator==(const BaseDequeIterator<T, IsConst1>& it1,
                const BaseDequeIterator<T, IsConst2>& it2);

template <typename T, bool IsConst1, bool IsConst2>
bool operator!=(const BaseDequeIterator<T, IsConst1>& it1,
                const BaseDequeIterator<T, IsConst1>& it2);

template <typename T>
class Deque {
 private:
  template <typename, bool>
  friend class BaseDequeIterator;

 public:
  using value_type = T;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using cosnt_pointer = const T*;
  using iterator = BaseDequeIterator<T, false>;
  using const_iterator = BaseDequeIterator<T, true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Deque();
  Deque(const Deque<T>& other);
  explicit Deque(size_type size);
  explicit Deque(size_type size, const_reference value);
  Deque& operator=(const Deque<T>& other);

  ~Deque();

  size_type size() const;

  reference operator[](size_type pos);
  const_reference operator[](size_type pos) const;

  reference at(size_type pos);
  const_reference at(size_type pos) const;

  void push_front(const_reference value);
  void pop_front();
  void push_back(const_reference value);
  void pop_back();

  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  const_reverse_iterator crbegin() const;

  iterator end();
  const_iterator end() const;
  const_iterator cend() const;
  reverse_iterator rend();
  const_reverse_iterator rend() const;
  const_reverse_iterator crend() const;

  void insert(iterator it, const_reference value);
  void insert(const_iterator it, const_reference value);

  void erase(const iterator& it);
  void erase(const_iterator it);

 private:
  std::pair<size_type, size_type> calc_indexes(size_type pos) const;
  void check_range(size_type pos) const;
  void destroy_n_elements(size_type n_elements);
  void create_data();

  static const size_type basket_size_ = 3;

  size_type n_baskets_;
  size_type first_pos_out_basket_;
  size_type first_pos_in_basket_;
  pointer* data_;
  size_type size_;
};

//=========================================================================================
//                            BaseDequeIterator DEFINITION
//=========================================================================================

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst>::BaseDequeIterator(size_type pos_out_basket,
                                                 size_type pos_in_basket,
                                                 const Deque<T>* deque)
    : pos_out_basket_(pos_out_basket),
      pos_in_basket_(pos_in_basket),
      deque_(deque) {}

template <typename T, bool IsConst>
inline BaseDequeIterator<T, IsConst>::BaseDequeIterator(
    const BaseDequeIterator<T, false>& it) {
  pos_out_basket_ = it.pos_out_basket_;
  pos_in_basket_ = it.pos_in_basket_;
  deque_ = it.deque_;
}

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst>& BaseDequeIterator<T, IsConst>::operator++() {
  ++pos_in_basket_;
  pos_out_basket_ += pos_in_basket_ / basket_size_;
  pos_in_basket_ %= basket_size_;
  return *this;
}

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst> BaseDequeIterator<T, IsConst>::operator++(int) {
  BaseDequeIterator tmp = *this;
  ++*this;
  return tmp;
}

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst>& BaseDequeIterator<T, IsConst>::operator--() {
  pos_out_basket_ =
      (pos_in_basket_ == 0) ? pos_out_basket_ - 1 : pos_out_basket_;
  pos_in_basket_ =
      (pos_in_basket_ == 0) ? basket_size_ - 1 : pos_in_basket_ - 1;
  return *this;
}

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst> BaseDequeIterator<T, IsConst>::operator--(int) {
  BaseDequeIterator tmp = *this;
  --*this;
  return tmp;
}

template <typename T, bool IsConst>
typename BaseDequeIterator<T, IsConst>::reference
BaseDequeIterator<T, IsConst>::operator*() const {
  return deque_->data_[pos_out_basket_][pos_in_basket_];
}

template <typename T, bool IsConst>
typename BaseDequeIterator<T, IsConst>::pointer
BaseDequeIterator<T, IsConst>::operator->() const {
  return &deque_->data_[pos_out_basket_][pos_in_basket_];
}

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst>& BaseDequeIterator<T, IsConst>::operator+=(
    difference_type diff) {
  size_type cur_pos = pos_out_basket_ * basket_size_ + pos_in_basket_;
  cur_pos += diff;
  pos_out_basket_ = cur_pos / basket_size_;
  pos_in_basket_ = cur_pos % basket_size_;
  return *this;
}
template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst>& BaseDequeIterator<T, IsConst>::operator-=(
    difference_type diff) {
  size_type cur_pos = pos_out_basket_ * basket_size_ + pos_in_basket_;
  cur_pos -= diff;
  pos_out_basket_ = cur_pos / basket_size_;
  pos_in_basket_ = cur_pos % basket_size_;
  return *this;
}

template <typename T, bool IsConst>
BaseDequeIterator<T, false> BaseDequeIterator<T, IsConst>::remove_const() {
  return BaseDequeIterator<T, false>(pos_out_basket_, pos_in_basket_, deque_);
}

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst> operator+(
    BaseDequeIterator<T, IsConst> it,
    typename BaseDequeIterator<T, IsConst>::difference_type diff) {
  it += diff;
  return it;
}

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst> operator+(
    typename BaseDequeIterator<T, IsConst>::difference_type diff,
    BaseDequeIterator<T, IsConst> it) {
  it += diff;
  return it;
}

template <typename T, bool IsConst>
BaseDequeIterator<T, IsConst> operator-(
    BaseDequeIterator<T, IsConst> it,
    typename BaseDequeIterator<T, IsConst>::difference_type diff) {
  it -= diff;
  return it;
}

template <typename T, bool IsConst1, bool IsConst2>
std::ptrdiff_t operator-(BaseDequeIterator<T, IsConst1> it1,
                         BaseDequeIterator<T, IsConst2> it2) {
  return (it1.pos_out_basket_ * it1.basket_size_ + it1.pos_in_basket_) -
         (it2.pos_out_basket_ * it2.basket_size_ + it2.pos_in_basket_);
}

template <typename T, bool IsConst1, bool IsConst2>
inline bool operator<(const BaseDequeIterator<T, IsConst1>& it1,
                      const BaseDequeIterator<T, IsConst2>& it2) {
  return it1.pos_out_basket_ * it1.basket_size_ + it1.pos_in_basket_ <
         it2.pos_out_basket_ * it2.basket_size_ + it2.pos_in_basket_;
}

template <typename T, bool IsConst1, bool IsConst2>
inline bool operator>(const BaseDequeIterator<T, IsConst1>& it1,
                      const BaseDequeIterator<T, IsConst2>& it2) {
  return it2 < it1;
}

template <typename T, bool IsConst1, bool IsConst2>
inline bool operator<=(const BaseDequeIterator<T, IsConst1>& it1,
                       const BaseDequeIterator<T, IsConst2>& it2) {
  return !(it1 > it2);
}

template <typename T, bool IsConst1, bool IsConst2>
inline bool operator>=(const BaseDequeIterator<T, IsConst1>& it1,
                       const BaseDequeIterator<T, IsConst2>& it2) {
  return !(it1 < it2);
}

template <typename T, bool IsConst1, bool IsConst2>
inline bool operator==(const BaseDequeIterator<T, IsConst1>& it1,
                       const BaseDequeIterator<T, IsConst2>& it2) {
  return it1.pos_out_basket_ * it1.basket_size_ + it1.pos_in_basket_ ==
         it2.pos_out_basket_ * it2.basket_size_ + it2.pos_in_basket_;
}

template <typename T, bool IsConst1, bool IsConst2>
inline bool operator!=(const BaseDequeIterator<T, IsConst1>& it1,
                       const BaseDequeIterator<T, IsConst2>& it2) {
  return !(it1 == it2);
}

//=========================================================================================
//                                  Deque DEFINITION
//=========================================================================================

template <typename T>
inline void Deque<T>::push_front(const_reference value) {
  if (first_pos_out_basket_ == 0 && first_pos_in_basket_ == 0) {
    pointer* new_data_ = reinterpret_cast<pointer*>(
        ::operator new((n_baskets_ + 1) * 2 * sizeof(pointer)));
    for (size_type from = 0, to = n_baskets_ + 2; from < n_baskets_;
         ++from, ++to) {
      new_data_[to] = data_[from];
    }
    for (size_type i = 0; i < n_baskets_ + 2; ++i) {
      new_data_[i] = reinterpret_cast<pointer>(
          ::operator new(basket_size_ * (sizeof(value_type))));
    }
    first_pos_out_basket_ = n_baskets_ + 2;
    n_baskets_ = (n_baskets_ + 1) * 2;
    ::operator delete(reinterpret_cast<void*>(data_));
    data_ = new_data_;
  }
  size_type pos_out_basket_ = (first_pos_in_basket_ == 0)
                                  ? first_pos_out_basket_ - 1
                                  : first_pos_out_basket_;
  size_type pos_in_basket_ =
      (first_pos_in_basket_ == 0) ? basket_size_ - 1 : first_pos_in_basket_ - 1;

  new (&data_[pos_out_basket_][pos_in_basket_]) value_type(value);
  first_pos_out_basket_ = pos_out_basket_;
  first_pos_in_basket_ = pos_in_basket_;
  ++size_;
}

template <typename T>
inline void Deque<T>::pop_front() {
  --size_;
  data_[first_pos_out_basket_][first_pos_in_basket_].~value_type();
  ++first_pos_in_basket_;
  first_pos_out_basket_ += first_pos_in_basket_ / basket_size_;
  first_pos_in_basket_ %= basket_size_;
}

template <typename T>
void Deque<T>::push_back(const_reference value) {
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(size_);
  if (pos_out_basket_ >= n_baskets_ - 1 && pos_in_basket_ >= basket_size_ - 1) {
    pointer* new_data_ = reinterpret_cast<pointer*>(
        ::operator new((n_baskets_ + 1) * 2 * sizeof(pointer)));
    for (size_type i = 0; i < n_baskets_; ++i) {
      new_data_[i] = data_[i];
    }
    for (size_type i = n_baskets_; i < (n_baskets_ + 1) * 2; ++i) {
      new_data_[i] = reinterpret_cast<pointer>(
          ::operator new(basket_size_ * (sizeof(value_type))));
    }
    n_baskets_ = (n_baskets_ + 1) * 2;
    ::operator delete(reinterpret_cast<void*>(data_));
    data_ = new_data_;
  }
  new (&data_[pos_out_basket_][pos_in_basket_]) value_type(value);
  ++size_;
}

template <typename T>
void Deque<T>::pop_back() {
  --size_;
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(size_);
  data_[pos_out_basket_][pos_in_basket_].~value_type();
}

template <typename T>
inline typename Deque<T>::iterator Deque<T>::begin() {
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(0);
  return iterator(pos_out_basket_, pos_in_basket_, this);
}

template <typename T>
inline typename Deque<T>::const_iterator Deque<T>::begin() const {
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(0);
  return const_iterator(pos_out_basket_, pos_in_basket_, this);
}

template <typename T>
inline typename Deque<T>::const_iterator Deque<T>::cbegin() const {
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(0);
  return const_iterator(pos_out_basket_, pos_in_basket_, this);
}

template <typename T>
inline typename Deque<T>::reverse_iterator Deque<T>::rbegin() {
  return std::reverse_iterator(end());
}

template <typename T>
inline typename Deque<T>::const_reverse_iterator Deque<T>::rbegin() const {
  return std::reverse_iterator(cend());
}

template <typename T>
inline typename Deque<T>::const_reverse_iterator Deque<T>::crbegin() const {
  return std::reverse_iterator(cend());
}

template <typename T>
inline typename Deque<T>::iterator Deque<T>::end() {
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(size_);
  return iterator(pos_out_basket_, pos_in_basket_, this);
}

template <typename T>
inline typename Deque<T>::const_iterator Deque<T>::end() const {
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(size_);
  return const_iterator(pos_out_basket_, pos_in_basket_, this);
}

template <typename T>
inline typename Deque<T>::const_iterator Deque<T>::cend() const {
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(size_);
  return const_iterator(pos_out_basket_, pos_in_basket_, this);
}

template <typename T>
inline typename Deque<T>::reverse_iterator Deque<T>::rend() {
  return std::reverse_iterator(begin());
}

template <typename T>
inline typename Deque<T>::const_reverse_iterator Deque<T>::rend() const {
  return std::reverse_iterator(cbegin());
}

template <typename T>
inline typename Deque<T>::const_reverse_iterator Deque<T>::crend() const {
  return std::reverse_iterator(cbegin());
}

template <typename T>
void Deque<T>::insert(iterator it, const_reference value) {
  if (it == begin()) {
    push_front(value);
  } else if (it == end()) {
    push_back(value);
  } else {
    push_back(value);
    reverse_iterator rit = std::reverse_iterator(it);
    for (reverse_iterator from = rbegin() + 1, to = rbegin(); from <= rit;
         ++from, ++to) {
      new (&(*to)) value_type(*from);
    }
    new (&(*it)) value_type(value);
  }
}

template <typename T>
void Deque<T>::insert(const_iterator cit, const_reference value) {
  insert(cit.remove_const(), value);
}

template <typename T>
void Deque<T>::erase(const iterator& it) {
  if (it == begin()) {
    pop_front();
  } else if (it == end() - 1) {
    pop_back();
  } else {
    it->~value_type();
    for (iterator from = it + 1, to = it; from != end(); ++from, ++to) {
      new (&(*(to))) value_type(*from);
    }
    (end() - 1)->~value_type();
    --size_;
  }
}

template <typename T>
void Deque<T>::erase(const_iterator cit) {
  erase(cit.remove_const());
}

template <typename T>
std::pair<typename Deque<T>::size_type, typename Deque<T>::size_type>
Deque<T>::calc_indexes(size_type pos) const {
  size_type pos_out_basket_ =
      first_pos_out_basket_ + (pos + first_pos_in_basket_) / basket_size_;
  size_type pos_in_basket_ = (pos + first_pos_in_basket_) % basket_size_;
  return {pos_out_basket_, pos_in_basket_};
}

template <typename T>
inline Deque<T>::Deque() {
  size_type size = 0;
  size_ = size;
  ++size;
  first_pos_out_basket_ = 0;
  first_pos_in_basket_ = 0;
  n_baskets_ = size / basket_size_ + ((size % basket_size_ == 0) ? 0 : 1);

  create_data();
}

template <typename T>
Deque<T>::Deque(const Deque<T>& other) {
  n_baskets_ = other.n_baskets_;
  first_pos_out_basket_ = other.first_pos_out_basket_;
  first_pos_in_basket_ = other.first_pos_in_basket_;
  size_ = other.size_;

  size_type counter = 0;
  try {
    create_data();
    for (; counter < size_; ++counter) {
      auto [pos_out_basket_, pos_in_basket_] = calc_indexes(counter);
      new (&data_[pos_out_basket_][pos_in_basket_])
          value_type(other.data_[pos_out_basket_][pos_in_basket_]);
    }
  } catch (...) {
    destroy_n_elements(counter);
    throw;
  }
}

template <typename T>
Deque<T>::Deque(size_type size) {
  size_ = size;
  ++size;
  first_pos_out_basket_ = 0;
  first_pos_in_basket_ = 0;
  n_baskets_ = size / basket_size_ + ((size % basket_size_ == 0) ? 0 : 1);

  size_type counter = 0;
  try {
    create_data();
    for (; counter < size_; ++counter) {
      auto [pos_out_basket_, pos_in_basket_] = calc_indexes(counter);
      new (&data_[pos_out_basket_][pos_in_basket_]) value_type();
    }
  } catch (...) {
    destroy_n_elements(counter);
    throw;
  }
}

template <typename T>
Deque<T>::Deque(size_type size, const_reference value) {
  size_ = size;
  ++size;
  first_pos_out_basket_ = 0;
  first_pos_in_basket_ = 0;
  n_baskets_ = size / basket_size_ + ((size % basket_size_ == 0) ? 0 : 1);

  size_type counter = 0;
  try {
    create_data();
    for (; counter < size_; ++counter) {
      auto [pos_out_basket_, pos_in_basket_] = calc_indexes(counter);
      new (&data_[pos_out_basket_][pos_in_basket_]) value_type(value);
    }
  } catch (...) {
    destroy_n_elements(counter);
    throw;
  }
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& other) {
  destroy_n_elements(size_);

  n_baskets_ = other.n_baskets_;
  first_pos_out_basket_ = other.first_pos_out_basket_;
  first_pos_in_basket_ = other.first_pos_in_basket_;
  size_ = other.size_;

  size_type counter = 0;
  try {
    create_data();
    for (; counter < size_; ++counter) {
      auto [pos_out_basket_, pos_in_basket_] = calc_indexes(counter);
      new (&data_[pos_out_basket_][pos_in_basket_])
          value_type(other.data_[pos_out_basket_][pos_in_basket_]);
    }
  } catch (...) {
    destroy_n_elements(counter);
    throw;
  }
  return *this;
}

template <typename T>
Deque<T>::~Deque() {
  destroy_n_elements(size_);
}

template <typename T>
typename Deque<T>::size_type Deque<T>::size() const {
  return size_;
}

template <typename T>
typename Deque<T>::reference Deque<T>::operator[](size_type pos) {
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(pos);
  return data_[pos_out_basket_][pos_in_basket_];
}

template <typename T>
typename Deque<T>::const_reference Deque<T>::operator[](size_type pos) const {
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(pos);
  return data_[pos_out_basket_][pos_in_basket_];
}

template <typename T>
typename Deque<T>::reference Deque<T>::at(size_type pos) {
  check_range(pos);
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(pos);
  return data_[pos_out_basket_][pos_in_basket_];
}

template <typename T>
typename Deque<T>::const_reference Deque<T>::at(size_type pos) const {
  check_range(pos);
  auto [pos_out_basket_, pos_in_basket_] = calc_indexes(pos);
  return data_[pos_out_basket_][pos_in_basket_];
}

template <typename T>
inline void Deque<T>::check_range(size_type pos) const {
  size_type lower_bound =
      first_pos_out_basket_ * basket_size_ + first_pos_in_basket_;
  size_type upper_bound = lower_bound + size_;
  if (pos < lower_bound || pos >= upper_bound) {
    throw std::out_of_range("index out of range");
  }
}

template <typename T>
inline void Deque<T>::destroy_n_elements(size_type n_elements) {
  for (size_type pos = 0; pos < n_elements; ++pos) {
    auto [pos_out_basket_, pos_in_basket_] = calc_indexes(pos);
    data_[pos_out_basket_][pos_in_basket_].~value_type();
  }
  for (size_type i = 0; i < n_baskets_; ++i) {
    ::operator delete(reinterpret_cast<void*>(data_[i]));
  }
  ::operator delete(reinterpret_cast<void*>(data_));
}

template <typename T>
inline void Deque<T>::create_data() {
  data_ =
      reinterpret_cast<pointer*>(::operator new(n_baskets_ * sizeof(pointer)));
  for (size_type i = 0; i < n_baskets_; ++i) {
    data_[i] = reinterpret_cast<pointer>(
        ::operator new(basket_size_ * (sizeof(value_type))));
  }
}
