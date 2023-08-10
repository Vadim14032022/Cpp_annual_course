#include <iostream>
#include <memory>

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

template <typename T>
class EnableSharedFromThis;

/**
================================================================================
                            START  ControlBlocks
================================================================================
*/

struct BaseControlBlock {
  size_t shared_ptr_count;
  size_t week_ptr_count;
  BaseControlBlock() : shared_ptr_count(1), week_ptr_count(1) {}
  virtual ~BaseControlBlock(){};
  virtual void dispose() = 0;
  virtual void destroy() = 0;
  virtual void* get() = 0;
  void release() {
    if (--shared_ptr_count == 0) {
      dispose();
      if (--week_ptr_count == 0) {
        destroy();
      }
    }
  }
  void add_ref() { ++shared_ptr_count; }
  void week_release() {
    if (--week_ptr_count == 0 && shared_ptr_count == 0) {
      destroy();
    }
  }
  void week_add_ref() { ++week_ptr_count; }
};

template <typename T, typename Alloc>
struct ControlBlockMakeShared : BaseControlBlock {
  template <typename... Args>
  ControlBlockMakeShared(Alloc alloc, Args&&... args) : alloc(alloc) {
    std::allocator_traits<Alloc>::construct(
        alloc, reinterpret_cast<T*>(&object), std::forward<Args>(args)...);
  }
  virtual void dispose() override {
    if (get() != nullptr) {
      std::allocator_traits<Alloc>::destroy(alloc,
                                            reinterpret_cast<T*>(&object));
    }
  }
  virtual void destroy() override {
    using AllocTraits = std::allocator_traits<Alloc>;
    using ControlBlockAllocTraits =
        typename AllocTraits::template rebind_traits<
            ControlBlockMakeShared<T, Alloc>>;
    using ControlBlockAlloc = typename AllocTraits::template rebind_alloc<
        ControlBlockMakeShared<T, Alloc>>;

    ControlBlockAlloc cb_alloc = alloc;
    ControlBlockAllocTraits::deallocate(cb_alloc, this, 1);
  }
  virtual void* get() override { return &object; }

  char object[sizeof(T)];
  Alloc alloc;
};

template <typename T, typename Deleter = std::default_delete<T>,
          typename Alloc = std::allocator<T>>
struct ControlBlock : BaseControlBlock {
  ControlBlock() : ptr(nullptr) {}
  ControlBlock(T* ptr) : ptr(ptr) {}
  ControlBlock(T* ptr, Deleter deleter) : ptr(ptr), deleter(deleter) {}
  ControlBlock(T* ptr, Deleter deleter, Alloc alloc)
      : ptr(ptr), deleter(deleter), alloc(alloc) {}
  virtual void dispose() override {
    if (ptr != nullptr) {
      deleter(ptr);
    }
  }
  virtual void destroy() override {
    using AllocTraits = std::allocator_traits<Alloc>;
    using ControlBlockAllocTraits =
        typename AllocTraits::template rebind_traits<
            ControlBlock<T, Deleter, Alloc>>;
    using ControlBlockAlloc = typename AllocTraits::template rebind_alloc<
        ControlBlock<T, Deleter, Alloc>>;

    ControlBlockAlloc cb_alloc = alloc;
    ControlBlockAllocTraits::deallocate(cb_alloc, this, 1);
  }
  virtual void* get() override { return ptr; }
  T* ptr;
  Deleter deleter;
  Alloc alloc;
};

/**
================================================================================
                            END  ControlBlocks
================================================================================
*/

/**
================================================================================
                            START  SharedPtr
================================================================================
*/

template <typename T>
class SharedPtr {
 public:
  SharedPtr() : cb_(nullptr) {}
  template <typename Y>
  SharedPtr(Y* c_ptr) : cb_(new ControlBlock<Y>(c_ptr)) {
    enable_shared_from_this_with<Y>();
  }
  template <typename Y, typename Deleter>
  SharedPtr(Y* c_ptr, Deleter deleter)
      : SharedPtr(c_ptr, deleter, std::allocator<void>()) {}
  template <typename Y, typename Deleter, typename Alloc>
  SharedPtr(Y* c_ptr, Deleter deleter, Alloc alloc) : cb_(nullptr) {
    using AllocTraits = std::allocator_traits<Alloc>;
    using ControlBlockAllocTraits =
        typename AllocTraits::template rebind_traits<
            ControlBlock<T, Deleter, Alloc>>;
    using ControlBlockAlloc = typename AllocTraits::template rebind_alloc<
        ControlBlock<T, Deleter, Alloc>>;

    ControlBlockAlloc cb_alloc = alloc;
    cb_ = ControlBlockAllocTraits::allocate(cb_alloc, 1);
    new (cb_) ControlBlock<T, Deleter, Alloc>(c_ptr, deleter, alloc);
    enable_shared_from_this_with<Y>();
  }

  template <typename Y>
  SharedPtr(const WeakPtr<Y>& w_ptr) : cb_(w_ptr.cb_) {
    if (cb_ != nullptr) cb_->add_ref();
  }
  template <typename Alloc>
  SharedPtr(ControlBlockMakeShared<T, Alloc>* cb) : cb_(cb) {
    enable_shared_from_this_with<T>();
  }

  SharedPtr(const SharedPtr& other) : cb_(other.cb_) {
    if (cb_ != nullptr) cb_->add_ref();
  }
  template <typename Y>
  SharedPtr(const SharedPtr<Y>& other) : cb_(other.cb_) {
    if (cb_ != nullptr) cb_->add_ref();
  }

  SharedPtr(SharedPtr&& other) : cb_(other.cb_) { other.fields_to_null(); }
  template <typename Y>
  SharedPtr(SharedPtr<Y>&& other) : cb_(other.cb_) {
    other.fields_to_null();
  }

  SharedPtr& operator=(const SharedPtr& other) {
    if (cb_ != nullptr) cb_->release();
    cb_ = other.cb_;
    if (cb_ != nullptr) cb_->add_ref();
    return *this;
  }
  template <typename Y>
  SharedPtr& operator=(const SharedPtr<Y>& other) {
    if (cb_ != nullptr) cb_->release();
    cb_ = other.cb_;
    if (cb_ != nullptr) cb_->add_ref();
    return *this;
  }

  SharedPtr& operator=(SharedPtr&& other) {
    if (cb_ != nullptr) cb_->release();
    cb_ = other.cb_;
    other.fields_to_null();
    return *this;
  }
  template <typename Y>
  SharedPtr& operator=(SharedPtr<Y>&& other) {
    if (cb_ != nullptr) cb_->release();
    cb_ = other.cb_;
    other.fields_to_null();
    return *this;
  }
  ~SharedPtr() {
    if (cb_ != nullptr) cb_->release();
  }

  T& operator*() const { return *get(); }
  T* operator->() const { return get(); }

  size_t use_count() const { return cb_->shared_ptr_count; }
  void reset() {
    if (cb_ != nullptr) cb_->release();
    fields_to_null();
  }
  template <typename Y>
  void reset(Y* c_ptr) {
    if (cb_ != nullptr) cb_->release();

    cb_ = new ControlBlock<Y>(c_ptr);
  }
  void swap(SharedPtr& other) { std::swap(cb_, other.cb_); }
  T* get() const { return cb_ ? reinterpret_cast<T*>(cb_->get()) : nullptr; }

  template <typename... Args>
  friend SharedPtr makeShared(Args&&... args);

  template <typename Alloc, typename... Args>
  friend SharedPtr alloccateShared(const Alloc& alloc, Args&&... args);

 private:
  template <typename>
  friend class WeakPtr;
  template <typename>
  friend class SharedPtr;
  template <typename>
  friend class EnableSharedFromThis;

  void fields_to_null() { cb_ = nullptr; }
  template <typename Y>
  void enable_shared_from_this_with() {
    if constexpr (std::is_base_of_v<EnableSharedFromThis<Y>, Y>) {
      get()->weak_this = *this;
    }
  }

  BaseControlBlock* cb_;
};

/**
================================================================================
                            END  SharedPtr
================================================================================
*/

/**
================================================================================
                            START  WeakPtr
================================================================================
*/

template <typename T>
class WeakPtr {
 public:
  WeakPtr() : cb_(nullptr) {}

  template <typename Y>
  WeakPtr(SharedPtr<Y> sh_ptr) : cb_(sh_ptr.cb_) {
    if (cb_ != nullptr) cb_->week_add_ref();
  }

  WeakPtr(const WeakPtr& other) : cb_(other.cb_) {
    if (cb_ != nullptr) cb_->week_add_ref();
  }
  template <typename Y>
  WeakPtr(const WeakPtr<Y>& other) : cb_(other.cb_) {
    if (cb_ != nullptr) cb_->week_add_ref();
  }

  WeakPtr(WeakPtr&& other) : cb_(other.cb_) { other.fields_to_null(); }
  template <typename Y>
  WeakPtr(WeakPtr<Y>&& other) : cb_(other.cb_) {
    other.fields_to_null();
  }

  WeakPtr& operator=(const WeakPtr& other) {
    if (cb_ != nullptr) cb_->week_release();
    cb_ = other.cb_;
    if (cb_ != nullptr) cb_->week_add_ref();
    return *this;
  }

  template <typename Y>
  WeakPtr& operator=(const WeakPtr<Y>& other) {
    if (cb_ != nullptr) cb_->week_release();
    cb_ = other.cb_;
    if (cb_ != nullptr) cb_->week_add_ref();
    return *this;
  }

  WeakPtr& operator=(WeakPtr&& other) {
    if (cb_ != nullptr) cb_->week_release();
    cb_ = other.cb_;
    other.fields_to_null();
    return *this;
  }
  template <typename Y>
  WeakPtr& operator=(WeakPtr<Y>&& other) {
    if (cb_ != nullptr) cb_->week_release();
    cb_ = other.cb_;
    other.fields_to_null();
    return *this;
  }
  template <typename Y>
  WeakPtr& operator=(const SharedPtr<Y>& sh_ptr) {
    if (cb_ != nullptr) cb_->week_release();
    cb_ = sh_ptr.cb_;
    if (cb_ != nullptr) cb_->week_add_ref();
    return *this;
  }
  ~WeakPtr() {
    if (cb_ != nullptr) cb_->week_release();
  }
  void swap(WeakPtr& other) { std::swap(cb_, other.cb_); }

  bool expired() const noexcept { return cb_->shared_ptr_count == 0; }

  SharedPtr<T> lock() const noexcept {
    return expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
  }

  size_t use_count() const { return cb_->shared_ptr_count; }

 private:
  template <typename>
  friend class WeakPtr;
  template <typename>
  friend class SharedPtr;
  template <typename>
  friend class EnableSharedFromThis;

  void fields_to_null() { cb_ = nullptr; }

  BaseControlBlock* cb_;
};

/**
================================================================================
                            END  WeakPtr
================================================================================
*/

/**
================================================================================
                            START  EnableSharedfromThis
================================================================================
*/

template <typename T>
class EnableSharedFromThis {
 protected:
  EnableSharedFromThis() {}
  EnableSharedFromThis(const EnableSharedFromThis& other) {}
  EnableSharedFromThis& operator=(const EnableSharedFromThis& other) {
    return *this;
  }
  ~EnableSharedFromThis() {}

 public:
  SharedPtr<T> shared_from_this() {
    return weak_this.cb_ == nullptr ? throw std::bad_weak_ptr()
                                    : weak_this.lock();
  }
  WeakPtr<T> week_from_this() { return weak_this; }

 private:
  template <typename>
  friend class WeakPtr;
  template <typename>
  friend class SharedPtr;
  template <typename>
  friend class EnableSharedFromThis;

  WeakPtr<T> weak_this;
};

/**
================================================================================
                            END  EnableSharedFromThis
================================================================================
*/

template <typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(const Alloc& alloc, Args&&... args) {
  using AllocTraits = std::allocator_traits<Alloc>;
  using ControlBlockAllocTraits = typename AllocTraits::template rebind_traits<
      ControlBlockMakeShared<T, Alloc>>;
  using ControlBlockAlloc = typename AllocTraits::template rebind_alloc<
      ControlBlockMakeShared<T, Alloc>>;

  ControlBlockAlloc cb_alloc = alloc;
  auto cb = ControlBlockAllocTraits::allocate(cb_alloc, 1);
  new (cb) ControlBlockMakeShared<T, Alloc>(alloc, std::forward<Args>(args)...);
  return SharedPtr<T>(cb);
}

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  using Alloc = std::allocator<T>;
  Alloc alloc;
  return allocateShared<T, Alloc>(alloc, std::forward<Args>(args)...);
}
