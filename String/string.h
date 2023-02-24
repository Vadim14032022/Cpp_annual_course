#include <algorithm>
#include <cstring>
#include <iostream>

class String {
 public:
  String();
  String(const char* str);
  String(const char* start, const char* end);
  String(size_t size, char letter);
  String(char letter);
  ~String() { delete[] data_; }
  String(const String& str);

  void push_back(char val);
  void pop_back();
  char& front();
  const char& front() const;
  char& back();
  const char& back() const;
  void swap(String& str);
  void clear();
  bool empty() const;
  char* data();
  const char* data() const;
  size_t length() const;
  size_t size() const;
  size_t capacity() const;

  size_t find(const String& str) const;
  size_t rfind(const String& str) const;
  String substr(size_t start, size_t count) const;
  void shrink_to_fit();

  String& operator=(const String& str);
  char operator[](size_t idx) const;
  char& operator[](size_t idx);
  String& operator+=(const String& str);

 private:
  size_t size_;
  size_t capacity_;
  char* data_;
};

String::String() : size_(0), capacity_(0), data_(nullptr) {}

String::String(const char* str) : size_(0) {
  while (str[size_] != '\0') {
    ++size_;
  }
  capacity_ = size_ + 1;
  data_ = new char[capacity_];
  std::copy(str, str + size_, data_);
  data_[size_] = '\0';
}

String::String(const char* start, const char* end) : size_(0) {
  while (start + size_ < end) {
    ++size_;
  }
  capacity_ = size_ + 1;
  data_ = new char[capacity_];
  std::copy(start, end, data_);
  data_[size_] = '\0';
}

String::String(size_t size, char letter)
    : size_(size), capacity_(size + 1), data_(new char[capacity_]) {
  std::fill(data_, data_ + size, letter);
  data_[size] = '\0';
}

String::String(char letter)
    : size_(1), capacity_(2), data_(new char[capacity_]) {
  data_[0] = letter;
  data_[size_] = '\0';
}

String::String(const String& str)
    : size_(str.size_), capacity_(str.capacity_), data_(new char[capacity_]) {
  std::copy(str.data_, str.data_ + size_, data_);
}

String& String::operator=(const String& str) {
  if (size_ >= str.size_) {
    size_ = str.size_;
    std::copy(str.data_, str.data_ + size_, data_);
    data_[size_] = '\0';
  } else {
    String(str).swap(*this);
  }

  return *this;
}
String& String::operator+=(const String& str) {
  if (str.size_ == 1) {
    push_back(str[0]);
  } else {
    capacity_ = (size_ + str.size_) * 2;
    char* new_data = new char[capacity_];
    std::copy(data_, data_ + size_, new_data);
    std::copy(str.data_, str.data_ + str.size_, new_data + size_);
    size_ += str.size_;
    new_data[size_] = '\0';
    delete[] data_;
    data_ = new_data;
  }
  return *this;
}

String operator+(const String& str1, const String& str2) {
  String cp(str1);
  return cp += str2;
}

bool operator<(const String& str1, const String& str2) {
  return strcmp(str1.data(), str2.data()) < 0;
}
bool operator>(const String& str1, const String& str2) { return str2 < str1; }

bool operator<=(const String& str1, const String& str2) {
  return !(str1 > str2);
}
bool operator>=(const String& str1, const String& str2) {
  return !(str2 < str1);
}
bool operator==(const String& str1, const String& str2) {
  if (str1.size() != str2.size()) {
    return false;
  }
  return strcmp(str1.data(), str2.data()) == 0;
}
bool operator!=(const String& str1, const String& str2) {
  return !(str1 == str2);
}

char String::operator[](size_t idx) const { return data_[idx]; }

char& String::operator[](size_t idx) { return data_[idx]; }

size_t String::length() const { return size_; }

size_t String::size() const { return size_; }

size_t String::capacity() const { return capacity_; }

void String::clear() { size_ = 0; }

bool String::empty() const { return size_ == 0; }

char* String::data() { return data_; }

const char* String::data() const { return data_; }

char& String::front() { return *data_; }

const char& String::front() const { return *data_; }

char& String::back() { return data_[size_ - 1]; }

const char& String::back() const { return data_[size_ - 1]; }

void String::swap(String& str) {
  std::swap(size_, str.size_);
  std::swap(capacity_, str.capacity_);
  std::swap(data_, str.data_);
}

void String::pop_back() { data_[--size_] = '\0'; }

void String::push_back(char val) {
  if (size_ == capacity_) {
    capacity_ = (capacity_ + 1) * 2;
    char* new_data = new char[capacity_];
    std::copy(data_, data_ + size_, new_data);
    delete[] data_;
    data_ = new_data;
  }
  data_[size_] = val;
  data_[++size_] = '\0';
}

size_t String::find(const String& str) const {
  size_t ans = size_;
  for (size_t i = 0; i < size_ - str.size_ + 1; ++i) {
    if (strncmp(this->data_ + i, str.data_, str.size_) == 0) {
      return i;
    }
  }
  return ans;
}
size_t String::rfind(const String& str) const {
  size_t ans = size_;
  for (size_t i = size_ - str.size_ + 2; i > 0; --i) {
    if (strncmp(this->data_ + i - 1, str.data_, str.size_) == 0) {
      return i - 1;
    }
  }
  return ans;
}
String String::substr(size_t start, size_t count) const {
  return String(data_ + start, data_ + start + count);
}
void String::shrink_to_fit() {
  capacity_ = size_;
  char* new_data = new char[capacity_];
  std::copy(data_, data_ + size_, new_data);
  delete[] data_;
  data_ = new_data;
  data_[size_] = '\0';
}

std::ostream& operator<<(std::ostream& os, const String& str) {
  for (size_t i = 0; i < str.size(); ++i) {
    os << str[i];
  }

  return os;
}

std::istream& operator>>(std::istream& is, String& str) {
  str.clear();
  char val;
  while (is.get(val) && !std::isspace(val)) {
    str.push_back(val);
  }
  return is;
}