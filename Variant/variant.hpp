#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <type_traits>

template <typename... Tail>
union VariadicUnion;

template <typename Head, typename... Tail>
union VariadicUnion<Head, Tail...>;

template <typename... Types>
class Variant;

static constexpr size_t bad_index = -1;
struct bad_type {};

template <size_t N>
struct in_place_index_t {
  explicit in_place_index_t() = default;
};

template <size_t N>
inline constexpr in_place_index_t<N> in_place_index{};

template <typename... Tail>
union VariadicUnion {
  VariadicUnion() {}
  ~VariadicUnion() {}

  VariadicUnion(const VariadicUnion& v_union) {}
  VariadicUnion(VariadicUnion&& v_union) {}

  template <size_t N>
  const auto& get() const {
    static_assert(N < 0, "N exceeds number of alternatives in variant");
    return *this;
  }
  template <size_t N>
  auto& get() {
    static_assert(N < 0, "N exceeds number of alternatives in variant");
    return *this;
  }
  template <typename U>
  void put(const U&) {
    static_assert(true, "No type in variant");
  }
  template <typename U>
  void put(U&&) {
    static_assert(true, "No type in variant");
  }
  template <size_t N>
  void put() {
    static_assert(true, "N exceeds number of alternatives in variant");
  }
  template <size_t N, typename... Args>
  auto& emplace(Args&&... args) {
    static_assert(
        true,
        "N exceeds number of alternatives in variant OR no type in variant");
    return *this;
  }
};

template <typename Head, typename... Tail>
union VariadicUnion<Head, Tail...> {
  Head head;
  VariadicUnion<Tail...> tail;

  VariadicUnion() {}
  ~VariadicUnion() {}
  template <typename U>
  VariadicUnion(const U& value) {
    if constexpr (std::is_convertible_v<U, Head>) {
      new ((void*)&head) Head(value);
    } else {
      new ((void*)&tail) VariadicUnion<Tail...>(value);
    }
  }

  template <typename U>
  VariadicUnion(U&& value) {
    if constexpr (std::is_convertible_v<U, Head>) {
      new ((void*)&head) Head(std::move(value));
    } else {
      new ((void*)&tail) VariadicUnion<Tail...>(std::move(value));
    }
  }

  VariadicUnion(const VariadicUnion& v_union) : head(v_union.head) {
    tail = v_union.tail;
  }

  VariadicUnion(VariadicUnion&& v_union) : head(std::move(v_union.head)) {
    tail = std::move(v_union.tail);
  }
  VariadicUnion& operator=(const VariadicUnion& v_union) { return *this; }
  VariadicUnion& operator=(VariadicUnion&& v_union) { return *this; }

  template <size_t N>
  VariadicUnion& operator=(const Head& value) {
    if constexpr (N == 0) {
      head = value;
      return *this;
    } else {
      return tail = value;
    }
  }
  template <size_t N>
  VariadicUnion& operator=(Head&& value) {
    if constexpr (N == 0) {
      head = std::move(value);
      return *this;
    } else {
      return tail = std::move(value);
    }
  }

  template <size_t N>
  const auto& get() const {
    if constexpr (N == 0) {
      return head;
    } else {
      return tail.template get<N - 1>();
    }
  }

  template <size_t N>
  auto& get() {
    if constexpr (N == 0) {
      return head;
    } else {
      return tail.template get<N - 1>();
    }
  }

  template <typename U>
  void put(const U& value) {
    if constexpr (std::is_same_v<U, Head> || std::is_assignable_v<Head, U>) {
      new ((void*)&head) U(value);
    } else {
      tail.template put<U>(value);
    }
  }

  template <typename U>
  void put(U&& value) {
    if constexpr (std::is_same_v<U, Head> || std::is_assignable_v<Head, U>) {
      new ((void*)&head) U(std::move(value));
    } else {
      tail.template put<U>(std::move(value));
    }
  }

  template <size_t N>
  void put() {
    if constexpr (N == 0) {
      new ((void*)&head) Head();
    } else {
      tail.template put<N - 1>();
    }
  }

  template <size_t N, typename... Args>
  void emplace(Args&&... args) {
    if constexpr (N == 0) {
      new ((void*)&head) Head(std::forward<Args>(args)...);
    } else {
      tail.template emplace<N - 1>(std::forward<Args>(args)...);
    }
  }

  template <size_t N, typename T>
  void accept(in_place_index_t<N>, const T& value) {
    tail.template accept(in_place_index<N - 1>, value);
  }
  template <typename T>
  void accept(in_place_index_t<0>, const T& value) {
    head = value;
  }

  template <size_t N, typename T>
  void accept(in_place_index_t<N>, T&& value) {
    tail.template accept(in_place_index<N - 1>, std::move(value));
  }
  template <typename T>
  void accept(in_place_index_t<0>, T&& value) {
    head = std::move(value);
  }

  template <typename T>
  void destroy() {
    if constexpr (std::is_same_v<T, Head>) {
      head.~Head();
    } else {
      tail.template destroy<T>();
    }
  }
};

template <size_t N, typename T, typename... Tail>
struct index_by_type_impl {
  static constexpr size_t value = bad_index;
};

template <size_t N, typename T, typename Head, typename... Tail>
struct index_by_type_impl<N, T, Head, Tail...> {
  static constexpr size_t value =
      std::is_same_v<T, Head> ? N
                              : index_by_type_impl<N + 1, T, Tail...>::value;
};

template <typename T, typename... Types>
static constexpr size_t index_by_type =
    index_by_type_impl<0, T, Types...>::value;

template <size_t N, typename... Tail>
struct type_by_index_impl {
  using type = bad_type;
};

template <size_t N, typename Head, typename... Tail>
struct type_by_index_impl<N, Head, Tail...> {
  using type =
      std::conditional_t<N == 0, Head, type_by_index_impl<N - 1, Tail...>>;
};

template <size_t N, typename... Types>
using type_by_index = typename type_by_index_impl<N, Types...>::type;

template <size_t N, typename T, typename... Tail>
struct convertible_index_impl {
  static constexpr size_t value = bad_index;
};

template <size_t N, typename T, typename Head, typename... Tail>
struct convertible_index_impl<N, T, Head, Tail...> {
  static constexpr size_t value =
      std::is_convertible_v<T, Head>
          ? N
          : convertible_index_impl<N + 1, T, Tail...>::value;
};

template <typename T, typename... Types>
static constexpr size_t convertible_index =
    convertible_index_impl<0, T, Types...>::value;

template <typename T, typename... Tail>
struct convertible_type_impl {
  using type = bad_type;
};

template <typename T, typename Head, typename... Tail>
struct convertible_type_impl<T, Head, Tail...> {
  using type =
      std::conditional_t<std::is_convertible_v<T, Head>, Head,
                         typename convertible_type_impl<T, Tail...>::type>;
};

template <typename T, typename... Types>
using convertible_type = typename convertible_type_impl<T, Types...>::type;

template <typename... Types>
class VariantStorage {
 private:
  template <typename... Ts>
  friend class VariantStorage;

  template <typename T, typename... Ts>
  friend class VariantAlternative;

  template <typename... Ts>
  friend class Variant;

  size_t active_index_ = 0;
  VariadicUnion<Types...> storage_;

 public:
  VariantStorage() { storage_.template put<0>(); }
  template <typename T>
  VariantStorage(const T& value)
      : active_index_(convertible_index<T, Types...>), storage_(value) {}
  template <typename T>
  VariantStorage(T&& value)
      : active_index_(convertible_index<T, Types...>),
        storage_(std::move(value)) {}
  ~VariantStorage() {}

  template <typename R_type, size_t N, typename Visitor>
  R_type storage_do_visit(Visitor&& vis) {
    switch (active_index_) {
      case 0:
        if constexpr (sizeof...(Types) > 0) {
          return std::invoke(std::forward<Visitor>(vis),
                             storage_.template get<0>());
        }
        break;
      case 1:
        if constexpr (sizeof...(Types) > 1) {
          return std::invoke(std::forward<Visitor>(vis),
                             storage_.template get<1>());
        }
        break;
      case 2:
        if constexpr (sizeof...(Types) > 2) {
          return std::invoke(std::forward<Visitor>(vis),
                             storage_.template get<2>());
        }
        break;
      case 3:
        if constexpr (sizeof...(Types) > 3) {
          return std::invoke(std::forward<Visitor>(vis),
                             storage_.template get<3>());
        }
        break;
      case 4:
        if constexpr (sizeof...(Types) > 4) {
          return std::invoke(std::forward<Visitor>(vis),
                             storage_.template get<4>());
        }
        break;
      case 5:
        if constexpr (sizeof...(Types) > 5) {
          return std::invoke(std::forward<Visitor>(vis),
                             storage_.template get<5>());
        }
        break;
      case 6:
        if constexpr (sizeof...(Types) > 6) {
          return std::invoke(std::forward<Visitor>(vis),
                             storage_.template get<6>());
        }
        break;
      case 7:
        if constexpr (sizeof...(Types) > 7) {
          return std::invoke(std::forward<Visitor>(vis),
                             storage_.template get<7>());
        }
        break;
      case 8:
        if constexpr (sizeof...(Types) > 8) {
          return std::invoke(std::forward<Visitor>(vis),
                             storage_.template get<8>());
        }
        break;
      case 9:
        if constexpr (sizeof...(Types) > 9) {
          return std::invoke(std::forward<Visitor>(vis),
                             storage_.template get<9>());
        }
        break;
      case 10:
        if constexpr (sizeof...(Types) > 10) {
          return std::invoke(std::forward<Visitor>(vis),
                             storage_.template get<10>());
        }
        break;
    }
    return R_type();
  }
  constexpr size_t get_item() {
    switch (active_index_) {
      case 0:
        if constexpr (sizeof...(Types) > 0) {
          return 0;
        }
        break;
    }
    return bad_index;
  }
};

template <typename T, typename... Types>
class VariantAlternative {
 private:
  template <typename... Ts>
  friend class VariantStorage;

  template <typename Y, typename... Ts>
  friend class VariantAlternative;

  template <typename... Ts>
  friend class Variant;

  using Derived = Variant<Types...>;

  static constexpr size_t index_ = index_by_type<T, Types...>;

 public:
  VariantAlternative() {}
  ~VariantAlternative() {}

  VariantAlternative(const T& value) {
    auto ptr = get_variant();
    ptr->storage_.template put<T>(value);
    ptr->get_active_index() = index_;
  }

  VariantAlternative(T&& value) {
    auto ptr = get_variant();
    ptr->storage_.template put<T>(std::move(value));
    ptr->get_active_index() = index_;
  }
  template <typename U,
            typename = std::enable_if_t<std::is_assignable_v<T, U> &&
                                        std::is_constructible_v<T, U>>>
  VariantAlternative(const U& value) {
    auto ptr = get_variant();
    ptr->storage_.template put<T>(value);
    ptr->get_active_index() = index_;
  }
  template <typename U,
            typename = std::enable_if_t<std::is_assignable_v<T, U> &&
                                        std::is_constructible_v<T, U>>>
  VariantAlternative(U&& value) {
    auto ptr = get_variant();
    ptr->storage_.template put<T>(std::move(value));
    ptr->get_active_index() = index_;
  }

  /* VariantAlternative(const VariantAlternative& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<const Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<T>(get<T>(*other_ptr));
      ptr->get_active_index() = other_ptr->active_index_;
    }
  }

  VariantAlternative(VariantAlternative&& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<T>(std::move(get<T>(*other_ptr)));
      ptr->get_active_index() = other_ptr->active_index_;
    }
  } */

  VariantAlternative(const VariantAlternative& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<const Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<T>(get<T>(*other_ptr));
      ptr->get_active_index() = other_ptr->active_index_;
    }
  }

  VariantAlternative(VariantAlternative&& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<T>(std::move(get<T>(*other_ptr)));
      ptr->get_active_index() = other_ptr->active_index_;
    }
  }

  VariantAlternative& operator=(const VariantAlternative& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<const Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<T>(get<T>(*other_ptr));
      ptr->get_active_index() = other_ptr->active_index_;
    }
    return *this;
  }

  VariantAlternative& operator=(VariantAlternative&& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<T>(std::move(get<T>(*other_ptr)));
      ptr->get_active_index() = other_ptr->active_index_;
    }
    return *this;
  }

  VariantAlternative& operator=(const T& value) {
    auto ptr = get_variant();
    if (ptr->get_active_index() == index_) {
      ptr->storage_.template accept(in_place_index<index_by_type<T, Types...>>,
                                    value);
    } else {
      ptr->destroy();
      ptr->storage_.template put<T>(value);
      ptr->get_active_index() = index_;
    }
    return *this;
  }
  VariantAlternative& operator=(T&& value) {
    auto ptr = get_variant();
    if (ptr->get_active_index() == index_) {
      ptr->storage_.template accept(in_place_index<index_by_type<T, Types...>>,
                                    std::move(value));
    } else {
      ptr->destroy();
      ptr->storage_.template put<T>(std::move(value));
      ptr->get_active_index() = index_;
    }
    return *this;
  }
  template <typename U,
            typename = std::enable_if_t<!std::is_trivial_v<U> &&
                                        (std::is_assignable_v<T, U> ||
                                         std::is_constructible_v<T, U>)>>
  VariantAlternative& operator=(const U& value) {
    auto ptr = get_variant();
    if (ptr->get_active_index() == index_) {
      ptr->storage_.template accept(in_place_index<index_by_type<T, Types...>>,
                                    value);
    } else {
      ptr->destroy();
      ptr->storage_.template put<T>(value);
      ptr->get_active_index() = index_;
    }
    return *this;
  }
  template <typename U,
            typename = std::enable_if_t<!std::is_trivial_v<U> &&
                                        (std::is_assignable_v<T, U> ||
                                         std::is_constructible_v<T, U>)>>
  VariantAlternative& operator=(U&& value) {
    auto ptr = get_variant();
    if (ptr->get_active_index() == index_) {
      ptr->storage_.template accept(in_place_index<index_by_type<T, Types...>>,
                                    std::move(value));
    } else {
      ptr->destroy();
      ptr->storage_.template put<T>(std::move(value));
      ptr->get_active_index() = index_;
    }
    return *this;
  }

  void destroy() {
    auto ptr = get_variant();
    if (ptr->active_index_ == index_) {
      ptr->get_storage().template destroy<T>();
    }
  }

  template <size_t N, std::enable_if_t<N == index_, bool> = true>
  constexpr size_t get_index() const {
    return index_;
  }

  Derived* get_variant() { return static_cast<Derived*>(this); }
};

template <typename T, typename... Types>
class VariantAlternative<const T, Types...> {
 private:
  template <typename... Ts>
  friend class VariantStorage;

  template <typename Y, typename... Ts>
  friend class VariantAlternative;

  template <typename... Ts>
  friend class Variant;

  using Derived = Variant<Types...>;

  static constexpr size_t index_ = index_by_type<const T, Types...>;

 public:
  VariantAlternative() {}
  ~VariantAlternative() {}
  VariantAlternative(const T& value) {
    auto ptr = get_variant();
    ptr->storage_.template put<const T>(value);
    ptr->get_active_index() = index_;
  }

  VariantAlternative(T&& value) {
    auto ptr = get_variant();
    ptr->storage_.template put<const T>(std::move(value));
    ptr->get_active_index() = index_;
  }

  VariantAlternative(const VariantAlternative& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<const Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<T>(get<T>(*other_ptr));
      ptr->get_active_index() = other_ptr->active_index_;
    }
  }

  VariantAlternative(VariantAlternative&& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<T>(std::move(get<T>(*other_ptr)));
      ptr->get_active_index() = other_ptr->active_index_;
    }
  }

  template <typename U/* , typename = std::enable_if_t<
                            index_ == VariantAlternative<U, Types...>::index_> */>
  VariantAlternative(const VariantAlternative<U, Types...>& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<const Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<const T>(get<const T>(*other_ptr));
      ptr->get_active_index() = other_ptr->active_index_;
    }
  }

  template <typename U/* , typename = std::enable_if_t<
                            index_ == VariantAlternative<U, Types...>::index_> */>
  VariantAlternative(VariantAlternative<U, Types...>&& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<const T>(std::move(get<const T>(*other_ptr)));
      ptr->get_active_index() = other_ptr->active_index_;
    }
  }

  template <typename U, typename = std::enable_if_t<
                            index_ == VariantAlternative<U, Types...>::index_>>
  VariantAlternative& operator=(const VariantAlternative<U, Types...>& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<const Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<T>(get<T>(*other_ptr));
      ptr->get_active_index() = other_ptr->active_index_;
    }
    return *this;
  }

  template <typename U, typename = std::enable_if_t<
                            index_ == VariantAlternative<U, Types...>::index_>>
  VariantAlternative& operator=(VariantAlternative<U, Types...>&& value) {
    auto ptr = get_variant();
    auto other_ptr = static_cast<Derived*>(&value);
    if (index_ == other_ptr->get_active_index()) {
      ptr->destroy();
      ptr->storage_.template put<T>(std::move(get<T>(*other_ptr)));
      ptr->get_active_index() = other_ptr->active_index_;
    }
    return *this;
  }

  VariantAlternative& operator=(const T& value) = delete;

  VariantAlternative& operator=(T&& value) = delete;

  void destroy() {
    auto ptr = get_variant();
    if (ptr->active_index_ == index_) {
      ptr->get_storage().template destroy<const T>();
    }
  }

  template <size_t N, std::enable_if_t<N == index_, bool> = true>
  constexpr size_t get_index() const {
    return index_;
  }

  Derived* get_variant() { return static_cast<Derived*>(this); }
};

template <typename... Types>
class Variant : private VariantStorage<Types...>,
                private VariantAlternative<Types, Types...>... {
 private:
  template <typename... Ts>
  friend class VariantStorage;

  template <typename T, typename... Ts>
  friend class VariantAlternative;

  template <typename... Ts>
  friend class Variant;

  template <size_t N, typename... Ts>
  friend constexpr auto& get(Variant<Ts...>&);

  template <size_t N, typename... Ts>
  friend constexpr auto&& get(Variant<Ts...>&&);

  template <size_t N, typename... Ts>
  friend constexpr const auto& get(const Variant<Ts...>& variant);

 public:
  using VariantAlternative<Types, Types...>::VariantAlternative...;
  using VariantAlternative<Types, Types...>::operator=...;

  Variant() {}
  ~Variant() { destroy(); }

  template <typename U, typename = std::enable_if_t<
                            std::is_same_v<U, const char[]> &&
                            (std::is_same_v<std::string, Types> || ...)>>
  Variant(const U& value) {}

  Variant(const Variant& variant)
      : VariantStorage<Types...>(),
        VariantAlternative<Types, Types...>(variant)... {}
  Variant(Variant&& variant)
      : VariantStorage<Types...>(),
        VariantAlternative<Types, Types...>(std::move(variant))... {}

  /* template <typename T,
            typename = std::enable_if_t<(std::is_same_v<Types, T> || ...)>>
  Variant& operator=(const T& value) {
    (VariantAlternative<Types, Types...>::operator=(value), ...);
    return *this;
  }

  template <typename T,
            typename = std::enable_if_t<(std::is_same_v<Types, T> || ...)>>
  Variant& operator=(T&& value) {
    (VariantAlternative<Types, Types...>::operator=(std::move(value)), ...);
    return *this;
  } */

  Variant& operator=(const Variant& variant) {
    (VariantAlternative<Types, Types...>::operator=(variant), ...);
    return *this;
  }

  Variant& operator=(Variant&& variant) {
    (VariantAlternative<Types, Types...>::operator=(std::move(variant)), ...);
    return *this;
  }

  template <size_t N, typename... Args>
  auto& emplace(Args&&... args) {
    destroy();
    this->active_index_ = N;
    this->storage_.template emplace<N>(std::forward<Args>(args)...);
    return get<N>(*this);
  }

  template <typename T, typename... Args>
  T& emplace(Args&&... args) {
    return emplace<index_by_type<T, Types...>>(std::forward<Args>(args)...);
  }

  template <size_t N, typename U, typename... Args>
  auto& emplace(std::initializer_list<U> i_list, Args&&... args) {
    destroy();
    this->active_index_ = N;
    this->storage_.template emplace<N>(i_list, std::forward<Args>(args)...);
    return get<N>(*this);
  }

  template <typename T, typename U, typename... Args>
  T& emplace(std::initializer_list<U> i_list, Args&&... args) {
    return emplace<index_by_type<T, Types...>>(i_list,
                                               std::forward<Args>(args)...);
  }

  template <typename Visitor>
  auto do_visit(Visitor&& vis) {
    using R_type = decltype(std::invoke(std::forward<Visitor>(vis),
                                        this->storage_.template get<0>()));
    return this->template storage_do_visit<R_type, 0>(
        std::forward<Visitor>(vis));
  }

  constexpr size_t index() const { return this->active_index_; }

  size_t packet_size() { return sizeof...(Types); }

  constexpr bool valueless_by_exception() const {
    return this->active_index_ != bad_index;
  }

 private:
  using VariantAlternative<Types, Types...>::get_index...;
  void destroy() { (VariantAlternative<Types, Types...>::destroy(), ...); }
  VariadicUnion<Types...>& get_storage() { return this->storage_; }
  size_t& get_active_index() { return this->active_index_; }
  const VariadicUnion<Types...>& get_storage() const { return this->storage_; }
  const size_t& get_active_index() const { return this->active_index_; }
};

template <size_t N, typename... Types>
constexpr auto& get(Variant<Types...>& variant) {
  static_assert(N < sizeof...(Types),
                "N exceeds number of alternatives in variant");
  if (variant.get_active_index() == N) {
    return variant.get_storage().template get<N>();
  }
  throw std::out_of_range("Bad variant access");
}

template <size_t N, typename... Types>
constexpr auto&& get(Variant<Types...>&& variant) {
  static_assert(N < sizeof...(Types),
                "N exceeds number of alternatives in variant");
  if (variant.get_active_index() == N) {
    return std::move(variant.get_storage().template get<N>());
  }
  throw std::out_of_range("Bad variant access");
}

template <size_t N, typename... Types>
constexpr const auto& get(const Variant<Types...>& variant) {
  static_assert(N < sizeof...(Types),
                "N exceeds number of alternatives in variant");
  if (variant.get_active_index() == N) {
    return variant.get_storage().template get<N>();
  }
  throw std::out_of_range("Bad variant access");
}

template <size_t N, typename... Types>
constexpr const auto&& get(const Variant<Types...>&& variant) {
  static_assert(N < sizeof...(Types),
                "N exceeds number of alternatives in variant");
  if (variant.get_active_index() == N) {
    return std::move(variant.get_storage().template get<N>());
  }
  throw std::out_of_range("Bad variant access");
}

template <typename T, typename... Types>
constexpr T& get(Variant<Types...>& variant) {
  return get<index_by_type<T, Types...>>(variant);
}

template <typename T, typename... Types>
constexpr T&& get(Variant<Types...>&& variant) {
  return std::move(get<index_by_type<T, Types...>>(variant));
}

template <typename T, typename... Types>
constexpr const T& get(const Variant<Types...>& variant) {
  return get<index_by_type<T, Types...>>(variant);
}

template <typename T, typename... Types>
constexpr const T&& get(const Variant<Types...>&& variant) {
  return std::move(get<index_by_type<T, Types...>>(variant));
}

template <typename T, typename... Types>
constexpr bool holds_alternative(Variant<Types...>& variant) {
  static_assert(bad_index != index_by_type<T, Types...>);
  return variant.index() == index_by_type<T, Types...>;
}

template <typename Visitor, typename... Variants>
auto visit(Visitor&& vis, Variants&&... variants) {
  return (variants.do_visit(std::forward<Visitor>(vis)), ...);
}
