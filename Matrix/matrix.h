#include <array>
#include <string>
#include <vector>

#pragma once

/*
================================================================================

                            UNSIGNED BIGINTEGER

================================================================================
*/

class UnsignedBigInteger {
 public:
  UnsignedBigInteger(uint64_t n = 1);
  UnsignedBigInteger(const std::string &str);
  UnsignedBigInteger(const UnsignedBigInteger &ubi) = default;
  ~UnsignedBigInteger() = default;

  void clear();
  UnsignedBigInteger &delete_leading_zeros();
  UnsignedBigInteger &add_trailing_zeros(uint64_t n);
  uint64_t DataSize() const;
  uint64_t count_nums() const;
  std::string toString() const;
  UnsignedBigInteger &shift_chunk(int64_t shift);
  UnsignedBigInteger times(uint64_t n) const;
  UnsignedBigInteger divide(uint64_t n) const;
  uint64_t size() const;
  static uint64_t chunk_size();

  UnsignedBigInteger &operator+=(const UnsignedBigInteger &ubi);
  UnsignedBigInteger &operator-=(const UnsignedBigInteger &ubi);
  UnsignedBigInteger &operator*=(const UnsignedBigInteger &ubi);
  UnsignedBigInteger &operator/=(const UnsignedBigInteger &ubi);
  UnsignedBigInteger &operator%=(const UnsignedBigInteger &ubi);
  UnsignedBigInteger &operator++();
  UnsignedBigInteger operator++(int);
  UnsignedBigInteger &operator--();
  UnsignedBigInteger operator--(int);
  UnsignedBigInteger operator-() const;
  explicit operator bool() const;

  friend bool operator<(const UnsignedBigInteger &ubi1,
                        const UnsignedBigInteger &ubi2);
  friend bool operator==(const UnsignedBigInteger &ubi1,
                         const UnsignedBigInteger &ubi2);

 private:
  UnsignedBigInteger(const std::vector<uint64_t> &v, size_t begin, size_t end);

  std::vector<uint64_t> data_;

  static uint64_t const chunk_ = 1000000000;

  static uint64_t const chunk_size_ = 9;
};

UnsignedBigInteger::UnsignedBigInteger(uint64_t n) : data_() {
  do {
    data_.push_back(n % chunk_);
    n /= chunk_;
  } while (n > 0);
}

UnsignedBigInteger::UnsignedBigInteger(const std::string &str)
    : data_(str.size() / chunk_size_ + ((str.size() % chunk_size_) ? 1 : 0)) {
  if (str[0] == '-') {
    throw std::invalid_argument("string start with '-'\n");
  }
  for (int64_t i = str.size() - chunk_size_, j = 0; i >= 0;
       i -= chunk_size_, ++j) {
    data_[j] = std::stoi(str.substr(i, chunk_size_));
  }
  if (str.size() % chunk_size_ != 0) {
    data_[str.size() / chunk_size_] =
        std::stoi(str.substr(0, str.size() % chunk_size_));
  }
  for (int64_t i = data_.size() - 1; i > 0 && data_[i] == 0; --i) {
    data_.pop_back();
  }
}

UnsignedBigInteger::UnsignedBigInteger(const std::vector<uint64_t> &v,
                                       size_t begin, size_t end)
    : data_(v.begin() + begin, v.begin() + end) {}

UnsignedBigInteger operator""_ubi(unsigned long long n) {
  return UnsignedBigInteger(std::to_string(n));
}

UnsignedBigInteger operator""_ubi(const char *n, size_t sz) {
  return UnsignedBigInteger(std::string(n, sz));
}

bool operator<(const UnsignedBigInteger &ubi1, const UnsignedBigInteger &ubi2) {
  if (ubi1.data_.size() != ubi2.data_.size()) {
    return ubi1.data_.size() < ubi2.data_.size();
  }
  for (int64_t i = ubi1.data_.size() - 1; i >= 0; --i) {
    if (ubi1.data_[i] != ubi2.data_[i]) {
      return ubi1.data_[i] < ubi2.data_[i];
    }
  }
  return false;
}

bool operator>(const UnsignedBigInteger &ubi1, const UnsignedBigInteger &ubi2) {
  return ubi2 < ubi1;
}

bool operator<=(const UnsignedBigInteger &ubi1,
                const UnsignedBigInteger &ubi2) {
  return !(ubi1 > ubi2);
}

bool operator>=(const UnsignedBigInteger &ubi1,
                const UnsignedBigInteger &ubi2) {
  return !(ubi1 < ubi2);
}

bool operator==(const UnsignedBigInteger &ubi1,
                const UnsignedBigInteger &ubi2) {
  if (ubi1.data_.size() != ubi2.data_.size()) {
    return false;
  }
  for (int64_t i = ubi1.data_.size() - 1; i >= 0; --i) {
    if (ubi1.data_[i] != ubi2.data_[i]) {
      return false;
    }
  }
  return true;
}

bool operator!=(const UnsignedBigInteger &ubi1,
                const UnsignedBigInteger &ubi2) {
  return !(ubi1 == ubi2);
}
UnsignedBigInteger::operator bool() const { return *this != 0; }

UnsignedBigInteger operator+(UnsignedBigInteger ubi1,
                             const UnsignedBigInteger &ubi2) {
  ubi1 += ubi2;
  return ubi1;
}

UnsignedBigInteger operator-(UnsignedBigInteger ubi1,
                             const UnsignedBigInteger &ubi2) {
  ubi1 -= ubi2;
  return ubi1;
}

UnsignedBigInteger operator*(UnsignedBigInteger ubi1,
                             const UnsignedBigInteger &ubi2) {
  ubi1 *= ubi2;
  return ubi1;
}

UnsignedBigInteger operator/(UnsignedBigInteger ubi1,
                             const UnsignedBigInteger &ubi2) {
  ubi1 /= ubi2;
  return ubi1;
}

UnsignedBigInteger operator%(UnsignedBigInteger ubi1,
                             const UnsignedBigInteger &ubi2) {
  ubi1 %= ubi2;
  return ubi1;
}

UnsignedBigInteger &UnsignedBigInteger::operator+=(
    const UnsignedBigInteger &ubi) {
  data_.resize(std::max(data_.size(), ubi.data_.size()), 0);

  uint64_t add = 0;
  for (size_t i = 0; i < data_.size(); ++i) {
    if (i < ubi.data_.size()) {
      data_[i] += ubi.data_[i];
    }
    data_[i] += add;
    add = data_[i] / chunk_;
    data_[i] %= chunk_;
  }
  if (add != 0) {
    data_.push_back(add);
  }

  return *this;
}

UnsignedBigInteger &UnsignedBigInteger::operator-=(
    const UnsignedBigInteger &ubi) {
  if (*this < ubi) {
    throw std::invalid_argument(
        "in UnsignedBigIntege::opertator-= catch *this < ubi\n" + toString() +
        " " + ubi.toString() + "\n");
  }
  uint64_t borrow = 0;
  uint64_t minus = 0;
  for (size_t i = 0; i < data_.size(); ++i) {
    if (i < ubi.data_.size()) {
      minus = ubi.data_[i];
    } else {
      minus = 0;
    }
    if (borrow + minus > data_[i]) {
      data_[i] = (chunk_ + data_[i]) - minus - borrow;
      borrow = 1;
    } else {
      data_[i] = data_[i] - minus - borrow;
      borrow = 0;
    }
  }

  delete_leading_zeros();

  return *this;
}
UnsignedBigInteger &UnsignedBigInteger::shift_chunk(int64_t shift) {
  std::vector<uint64_t> v(shift, 0);
  v.insert(v.end(), data_.begin(), data_.end());
  data_ = v;
  return *this;
}

UnsignedBigInteger &UnsignedBigInteger::operator*=(
    const UnsignedBigInteger &ubi) {
  UnsignedBigInteger res = *this;
  *this = 0;
  for (size_t i = 0; i < ubi.data_.size(); ++i) {
    *this += res.times(ubi.data_[i]).shift_chunk(i);
  }
  delete_leading_zeros();

  return *this;
}
UnsignedBigInteger UnsignedBigInteger::times(uint64_t n) const {
  UnsignedBigInteger ubi = *this;
  uint64_t add = 0;
  for (size_t i = 0; i < data_.size(); ++i) {
    ubi.data_[i] *= n;
    ubi.data_[i] += add;
    add = ubi.data_[i] / chunk_;
    ubi.data_[i] %= chunk_;
  }
  if (add != 0) {
    ubi.data_.push_back(add);
  }
  return ubi.delete_leading_zeros();
}
UnsignedBigInteger UnsignedBigInteger::divide(uint64_t n) const {
  std::vector<uint64_t> ans(data_.size(), 0);
  uint64_t add = 0;
  for (int64_t i = data_.size() - 1; i >= 0; --i) {
    ans[i] = (add + data_[i]) / n;
    add = ((add + data_[i]) % n) * chunk_;
  }
  UnsignedBigInteger ubi(ans, 0, ans.size());
  ubi.delete_leading_zeros();
  return ubi;
}

UnsignedBigInteger &UnsignedBigInteger::operator/=(
    const UnsignedBigInteger &ubi) {
  if (*this < ubi) {
    *this = 0;
    return *this;
  }

  std::vector<uint64_t> ans(data_.size() - ubi.data_.size() + 1, 0);
  for (int64_t i = ans.size() - 1; i >= 0; --i) {
    uint64_t start = 0;
    uint64_t end = chunk_ - 1;

    while (end - start > 1) {
      uint64_t mid = (start + end) / 2;
      UnsignedBigInteger num = ubi.times(mid);
      num.shift_chunk(i);
      if (num < *this) {
        start = mid;
      } else {
        end = mid;
      }
    }

    if (ubi.times(std::max(start, end)).shift_chunk(i) <= *this) {
      ans[i] = std::max(start, end);
    } else {
      ans[i] = std::min(start, end);
    }
    *this -= ubi.times(ans[i]).shift_chunk(i).delete_leading_zeros();
  }
  *this = UnsignedBigInteger(ans, 0, ans.size());
  delete_leading_zeros();

  return *this;
}

UnsignedBigInteger &UnsignedBigInteger::operator%=(
    const UnsignedBigInteger &ubi) {
  *this -= (*this / ubi) * ubi;
  return *this;
}

UnsignedBigInteger &UnsignedBigInteger::operator++() {
  *this += 1;
  return *this;
}

UnsignedBigInteger UnsignedBigInteger::operator++(int) {
  UnsignedBigInteger copy = *this;
  ++*this;
  return copy;
}

UnsignedBigInteger &UnsignedBigInteger::operator--() {
  *this -= 1;
  return *this;
}

UnsignedBigInteger UnsignedBigInteger::operator--(int) {
  UnsignedBigInteger copy = *this;
  --*this;
  return copy;
}

void UnsignedBigInteger::clear() {
  data_.resize(0);
  data_.shrink_to_fit();
}

UnsignedBigInteger &UnsignedBigInteger::delete_leading_zeros() {
  for (int64_t i = data_.size() - 1; i > 0 && data_[i] == 0; --i) {
    data_.pop_back();
  }
  return *this;
}

std::string UnsignedBigInteger::toString() const {
  std::string str = "";
  for (size_t i = data_.size(); i >= 1; --i) {
    std::string k = std::to_string(data_[i - 1]);
    if (i != data_.size()) {
      str += std::string(chunk_size_ - k.size(), '0');
    }
    str += k;
  }
  return str;
}

uint64_t UnsignedBigInteger::size() const { return data_.size(); }

uint64_t UnsignedBigInteger::chunk_size() { return chunk_size_; }

std::ostream &operator<<(std::ostream &os, const UnsignedBigInteger &ubi) {
  os << ubi.toString();
  return os;
}

std::istream &operator>>(std::istream &is, UnsignedBigInteger &ubi) {
  ubi.clear();
  std::string input;
  is >> input;
  ubi = input;
  return is;
}

/*
================================================================================

                                BIGINTEGER

================================================================================
*/

class BigInteger {
  enum class Type { negative = 0, positive = 1 };

 public:
  BigInteger(int64_t n = 1);
  BigInteger(const std::string &str);
  BigInteger(const UnsignedBigInteger &ubi);
  BigInteger(const BigInteger &bi) = default;
  ~BigInteger() = default;

  std::string toString() const;
  void Invert();
  void clear();
  UnsignedBigInteger abs() const;
  Type sign() const;
  BigInteger &shift_chunk(int64_t shift);
  uint64_t size() const;
  static uint64_t chunk_size();

  BigInteger &operator+=(const BigInteger &bi);
  BigInteger &operator-=(const BigInteger &bi);
  BigInteger &operator*=(const BigInteger &bi);
  BigInteger &operator/=(const BigInteger &bi);
  BigInteger &operator%=(const BigInteger &bi);
  BigInteger &operator++();
  BigInteger operator++(int);
  BigInteger &operator--();
  BigInteger operator--(int);
  BigInteger operator-() const;
  explicit operator bool() const;

  friend bool operator<(const BigInteger &bi1, const BigInteger &bi2);
  friend bool operator==(const BigInteger &bi1, const BigInteger &bi2);

 private:
  UnsignedBigInteger ubi_;
  Type type_;
};

BigInteger::BigInteger(int64_t n)
    : ubi_(std::abs(n)), type_((n < 0) ? Type::negative : Type::positive) {}

BigInteger::BigInteger(const std::string &str)
    : ubi_((str[0] == '-') ? str.substr(1) : str),
      type_((str[0] == '-') ? Type::negative : Type::positive) {}

BigInteger::BigInteger(const UnsignedBigInteger &ubi)
    : ubi_(ubi), type_(Type::positive) {}

BigInteger operator""_bi(unsigned long long n) {
  return BigInteger(std::to_string(n));
}

BigInteger operator""_bi(const char *n, size_t sz) {
  return BigInteger(std::string(n, sz));
}

bool operator<(const BigInteger &bi1, const BigInteger &bi2) {
  if (bi1.type_ != bi2.type_) {
    return bi1.type_ < bi2.type_;
  }
  if (bi1.type_ == BigInteger::Type::negative) {
    return bi2.ubi_ < bi1.ubi_;
  }
  return bi1.ubi_ < bi2.ubi_;
}

bool operator>(const BigInteger &bi1, const BigInteger &bi2) {
  return bi2 < bi1;
}

bool operator<=(const BigInteger &bi1, const BigInteger &bi2) {
  return !(bi1 > bi2);
}

bool operator>=(const BigInteger &bi1, const BigInteger &bi2) {
  return !(bi1 < bi2);
}

bool operator==(const BigInteger &bi1, const BigInteger &bi2) {
  if (bi1.type_ != bi2.type_) {
    return false;
  }
  return bi1.ubi_ == bi2.ubi_;
}

bool operator!=(const BigInteger &bi1, const BigInteger &bi2) {
  return !(bi1 == bi2);
}

BigInteger::operator bool() const { return *this != 0; }

BigInteger operator+(BigInteger bi1, const BigInteger &bi2) {
  bi1 += bi2;
  return bi1;
}

BigInteger operator-(BigInteger bi1, const BigInteger &bi2) {
  bi1 -= bi2;
  return bi1;
}

BigInteger operator*(BigInteger bi1, const BigInteger &bi2) {
  bi1 *= bi2;
  return bi1;
}

BigInteger operator/(BigInteger bi1, const BigInteger &bi2) {
  bi1 /= bi2;
  return bi1;
}

BigInteger operator%(BigInteger bi1, const BigInteger &bi2) {
  bi1 %= bi2;
  return bi1;
}

BigInteger &BigInteger::operator+=(const BigInteger &bi) {
  if (type_ == bi.type_) {
    ubi_ += bi.ubi_;
  } else {
    if (ubi_ >= bi.ubi_) {
      ubi_ -= bi.ubi_;
    } else {
      ubi_ = bi.ubi_ - ubi_;
      if (type_ == Type::positive) {
        type_ = Type::negative;
      } else {
        type_ = Type::positive;
      }
    }
  }

  if (ubi_ == 0) {
    type_ = Type::positive;
  }

  return *this;
}

BigInteger &BigInteger::operator-=(const BigInteger &bi) {
  return *this += (-bi);
}

BigInteger &BigInteger::operator*=(const BigInteger &bi) {
  ubi_ *= bi.ubi_;
  if (type_ == bi.type_) {
    type_ = Type::positive;
  } else {
    type_ = Type::negative;
  }

  if (ubi_ == 0) {
    type_ = Type::positive;
  }

  return *this;
}

BigInteger &BigInteger::operator/=(const BigInteger &bi) {
  ubi_ /= bi.ubi_;
  if (type_ == bi.type_) {
    type_ = Type::positive;
  } else {
    type_ = Type::negative;
  }

  if (ubi_ == 0) {
    type_ = Type::positive;
  }

  return *this;
}

BigInteger &BigInteger::operator%=(const BigInteger &bi) {
  return *this -= (*this / bi) * bi;
}

BigInteger &BigInteger::operator++() {
  *this += 1;
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger copy = *this;
  ++*this;
  return copy;
}

BigInteger &BigInteger::operator--() {
  *this -= 1;
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger copy = *this;
  --*this;
  return copy;
}

BigInteger BigInteger::operator-() const {
  BigInteger copy = *this;
  copy.Invert();
  return copy;
}

std::string BigInteger::toString() const {
  std::string str = (type_ == Type::negative) ? "-" : "";
  str += ubi_.toString();
  return str;
}

void BigInteger::Invert() {
  if (type_ == Type::positive && *this != 0) {
    type_ = Type::negative;
  } else {
    type_ = Type::positive;
  }
}

void BigInteger::clear() { ubi_.clear(); }

UnsignedBigInteger BigInteger::abs() const { return ubi_; }
BigInteger::Type BigInteger::sign() const { return type_; }

BigInteger &BigInteger::shift_chunk(int64_t shift) {
  ubi_.shift_chunk(shift);
  return *this;
}

uint64_t BigInteger::chunk_size() { return UnsignedBigInteger::chunk_size(); }

uint64_t BigInteger::size() const { return ubi_.size(); }

std::ostream &operator<<(std::ostream &os, const BigInteger &bi) {
  os << bi.toString();
  return os;
}

std::istream &operator>>(std::istream &is, BigInteger &bi) {
  bi.clear();
  std::string input;
  is >> input;
  bi = input;
  return is;
}

/*
================================================================================

                                  RATIONAL

================================================================================
*/

class Rational {
 public:
  Rational(int64_t n = 1);
  Rational(const BigInteger &bi);
  Rational &operator+=(const Rational &r);
  Rational &operator-=(const Rational &r);
  Rational &operator*=(const Rational &r);
  Rational &operator/=(const Rational &r);
  Rational operator-() const;
  explicit operator double();

  friend bool operator<(const Rational &r1, const Rational &r2);
  friend bool operator==(const Rational &r1, const Rational &r2);

  std::string toString();
  std::string asDecimal(size_t precision);
  void correct_sign();
  void reduction();

 private:
  BigInteger numerator;
  BigInteger denominator;
};

Rational::Rational(int64_t n) : numerator(n), denominator(1) {}
Rational::Rational(const BigInteger &bi) : numerator(bi), denominator(1) {}

bool operator<(const Rational &r1, const Rational &r2) {
  if (r1.numerator.sign() != r2.numerator.sign()) {
    return r1.numerator.sign() < r2.numerator.sign();
  }
  return r1.numerator * r2.denominator < r2.numerator * r1.denominator;
}

bool operator>(const Rational &r1, const Rational &r2) { return r2 < r1; }

bool operator<=(const Rational &r1, const Rational &r2) { return !(r1 > r2); }

bool operator>=(const Rational &r1, const Rational &r2) { return !(r1 < r2); }

bool operator==(const Rational &r1, const Rational &r2) {
  return r1.numerator * r2.denominator == r2.numerator * r1.denominator;
}

bool operator!=(const Rational &r1, const Rational &r2) { return !(r1 == r2); }

Rational &Rational::operator+=(const Rational &r) {
  numerator *= r.denominator;
  numerator += denominator * r.numerator;
  denominator *= r.denominator;
  correct_sign();
  reduction();
  return *this;
}
Rational &Rational::operator-=(const Rational &r) {
  *this += (-r);
  return *this;
}
Rational &Rational::operator*=(const Rational &r) {
  numerator *= r.numerator;
  denominator *= r.denominator;
  correct_sign();
  reduction();
  return *this;
}
Rational &Rational::operator/=(const Rational &r) {
  numerator *= r.denominator;
  denominator *= r.numerator;
  correct_sign();
  reduction();
  return *this;
}
Rational Rational::operator-() const {
  Rational copy = *this;
  copy.numerator.Invert();
  return copy;
}

Rational operator+(Rational r1, const Rational &r2) {
  r1 += r2;
  return r1;
}
Rational operator-(Rational r1, const Rational &r2) {
  r1 -= r2;
  return r1;
}
Rational operator*(Rational r1, const Rational &r2) {
  r1 *= r2;
  return r1;
}
Rational operator/(Rational r1, const Rational &r2) {
  r1 /= r2;
  return r1;
}

std::string Rational::toString() {
  // reduction();
  std::string str = numerator.toString();
  if (denominator != 1) {
    str += "/";
    str += denominator.toString();
  }
  return str;
}

std::string Rational::asDecimal(size_t precision = 0) {
  // reduction();
  std::string str = (numerator < 0) ? "-" : "";
  BigInteger numer = numerator.abs();
  str += (numer / denominator).toString();
  if (precision > 0) {
    str += ".";
    std::string addition = "";
    while (addition.size() <= precision) {
      numer %= denominator;
      numer *= 10;
      addition += (numer / denominator).toString();
    }
    /*if (addition[addition.size() - 1] - '0' >= 5 &&
        addition[addition.size() - 1] != '9') {
      addition[addition.size() - 2] += 1;
    }*/
    addition.pop_back();

    str += addition;
  }
  return str;
}

void Rational::correct_sign() {
  if (denominator < 0) {
    if (numerator != 0) {
      numerator.Invert();
    }
    denominator.Invert();
  }
}

void Rational::reduction() {
  correct_sign();
  UnsignedBigInteger a = numerator.abs();
  UnsignedBigInteger b = denominator.abs();
  while (a != 0 && b != 0) {
    if (a > b) {
      a %= b;
    } else {
      b %= a;
    }
  }
  numerator /= (a + b);
  denominator /= (a + b);
}

Rational::operator double() { return std::stod(asDecimal(10)); }

std::ostream &operator<<(std::ostream &os, const Rational &r) {
  os << Rational(r).asDecimal();
  return os;
}

std::istream &operator>>(std::istream &is, Rational &r) {
  std::string in;
  is >> in;
  std::string numer = in;
  std::string denom = "";
  r = 1;
  for (size_t i = 0; i < in.size(); ++i) {
    if (in[i] == '/') {
      numer = in.substr(0, i);
    }
  }
  r *= BigInteger(numer);
  if (numer.size() != in.size()) {
    denom = in.substr(numer.size() + 1);
    r /= BigInteger(denom);
  }
  return is;
}

/*
================================================================================

                                    RESIDUE

================================================================================
*/

template <size_t N, size_t S, bool end = (S * S > N)>
struct sqrt_search {
  static const size_t value =
      sqrt_search<N, S + 1, ((S + 1) * (S + 1) > N)>::value;
};

template <size_t N, size_t S>
struct sqrt_search<N, S, true> {
  static const size_t value = S - 1;
};

template <size_t N>
const size_t Sqrt = sqrt_search<N, 0>::value;

template <size_t N, size_t S>
struct prime_check {
  static const bool value = (N % S == 0) ? false : prime_check<N, S - 1>::value;
};

template <size_t N>
struct prime_check<N, 1> {
  static const bool value = true;
};

template <size_t N>
const bool is_prime = prime_check<N, Sqrt<N>>::value;

template <size_t N>
class Residue {
 public:
  Residue(int n = 0);
  operator int() const;
  Residue<N> &operator+=(const Residue<N> &r);
  Residue<N> &operator-=(const Residue<N> &r);
  Residue<N> &operator*=(const Residue<N> &r);
  Residue<N> &operator/=(const Residue<N> &r);

  size_t val_;
};

template <size_t N>
Residue<N>::Residue(int n)
    : val_((n >= 0) ? n % N : (n + (abs(n) / N + 1) * N) % N) {}

template <size_t N>
Residue<N>::operator int() const {
  return static_cast<int>(val_);
}
template <size_t N>
Residue<N> &Residue<N>::operator+=(const Residue<N> &r) {
  val_ += r.val_;
  val_ %= N;
  return *this;
}
template <size_t N>
Residue<N> &Residue<N>::operator-=(const Residue<N> &r) {
  if (r.val_ > val_) {
    val_ += N;
  }
  val_ -= r.val_;
  val_ %= N;
  return *this;
}
template <size_t N>
Residue<N> &Residue<N>::operator*=(const Residue<N> &r) {
  val_ *= r.val_;
  val_ %= N;
  return *this;
}

size_t gcd_ext(int64_t a, int64_t b, int64_t &x, int64_t &y) {
  if (b == 0) {
    x = 1;
    y = 0;
    return a;
  }
  long long d = gcd_ext(b, a % b, x, y);
  x -= (a / b) * y;
  std::swap(x, y);
  return d;
}

template <size_t N>
Residue<N> &Residue<N>::operator/=(const Residue<N> &r) {
  static_assert(is_prime<N>, "your number isn't prime");
  int64_t x = 0, y = 0;
  gcd_ext(r.val_, N, x, y);
  x *= val_;
  x += (abs(x) / N + 1) * N;
  val_ = x % N;
  return *this;
}
template <size_t N>
Residue<N> operator+(Residue<N> r1, const Residue<N> &r2) {
  return r1 += r2;
}
template <size_t N>
Residue<N> operator-(Residue<N> r1, const Residue<N> &r2) {
  return r1 -= r2;
}
template <size_t N>
Residue<N> operator*(Residue<N> r1, const Residue<N> &r2) {
  return r1 *= r2;
}
template <size_t N>
Residue<N> operator/(Residue<N> r1, const Residue<N> &r2) {
  return r1 /= r2;
}

template <size_t N>
bool operator==(const Residue<N> &r1, const Residue<N> &r2) {
  return r1.val_ == r2.val_;
}

template <size_t N>
bool operator!=(const Residue<N> &r1, const Residue<N> &r2) {
  return !(r1 == r2);
}
/*
================================================================================

                                    MATRIX

================================================================================
*/

template <size_t M, size_t N, typename Field = Rational>
class Matrix {
 public:
  Matrix();
  Matrix(const std::initializer_list<std::initializer_list<Field>> &list);
  Matrix(const Matrix<M, N, Field> &other);

  Matrix<M, N, Field> &operator+=(const Matrix<M, N, Field> &m);
  Matrix<M, N, Field> &operator-=(const Matrix<M, N, Field> &m);
  Matrix<M, N, Field> &operator*=(const Field &f);
  Matrix<M, N, Field> &operator*=(const Matrix<M, N, Field> &m);

  Matrix<N, M, Field> transposed() const;
  Field det() const;
  size_t rank() const;
  Matrix<M, N, Field> inverted() const;
  void invert();
  Field trace() const;
  std::array<Field, N> getRow(size_t idx) const;
  std::array<Field, M> getColumn(size_t idx) const;
  std::array<Field, N> &operator[](size_t idx);
  const std::array<Field, N> &operator[](size_t idx) const;

  std::array<std::array<Field, N>, M> data_;
};

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix() {
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      data_[i][j] = Field(0);
    }
  }
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix(
    const std::initializer_list<std::initializer_list<Field>> &list) {
  size_t i = 0;

  for (auto &row : list) {
    size_t j = 0;
    for (auto &el : row) {
      data_[i][j] = el;
      ++j;
    }
    ++i;
  }
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix(const Matrix<M, N, Field> &other) {
  data_ = other.data_;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> &Matrix<M, N, Field>::operator+=(
    const Matrix<M, N, Field> &m) {
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      data_[i][j] += m.data_[i][j];
    }
  }
  return *this;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> &Matrix<M, N, Field>::operator-=(
    const Matrix<M, N, Field> &m) {
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      data_[i][j] -= m.data_[i][j];
    }
  }
  return *this;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> &Matrix<M, N, Field>::operator*=(const Field &f) {
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      data_[i][j] *= f;
    }
  }
  return *this;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator+(Matrix<M, N, Field> m1,
                              const Matrix<M, N, Field> &m2) {
  return m1 += m2;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator-(Matrix<M, N, Field> m1,
                              const Matrix<M, N, Field> &m2) {
  return m1 -= m2;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator*(Matrix<M, N, Field> m, const Field &f) {
  return m *= f;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator*(const Field &f, Matrix<M, N, Field> m) {
  return m *= f;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> &Matrix<M, N, Field>::operator*=(
    const Matrix<M, N, Field> &m1) {
  static_assert(M == N);
  Matrix<M, M, Field> res;
  for (size_t k = 0; k < M; ++k) {
    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < M; ++n) {
        res.data_[m][k] += data_[m][n] * m1.data_[n][k];
      }
    }
  }
  this->data_ = res.data_;
  return *this;
}

template <size_t M, size_t N, size_t K, typename Field>
Matrix<M, K, Field> operator*(const Matrix<M, N, Field> &m1,
                              const Matrix<N, K, Field> &m2) {
  Matrix<M, K, Field> res;
  for (size_t k = 0; k < K; ++k) {
    for (size_t m = 0; m < M; ++m) {
      for (size_t n = 0; n < N; ++n) {
        res.data_[m][k] += m1.data_[m][n] * m2.data_[n][k];
      }
    }
  }
  return res;
}

template <size_t M, size_t N, typename Field>
Matrix<N, M, Field> Matrix<M, N, Field>::transposed() const {
  Matrix<N, M, Field> t;
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      t.data_[j][i] = data_[i][j];
    }
  }
  return t;
}

Rational abs(Rational r) { return (r > 0) ? r : -r; }

template <size_t M, size_t N, typename Field>
Field Matrix<M, N, Field>::det() const {
  static_assert(M == N);
  Field ans(1);
  Matrix<M, N, Field> tmp(*this);
  for (size_t col = 0; col < M; ++col) {
    size_t max_row = col;
    for (size_t under_row = col + 1; under_row < M; ++under_row) {
      if (abs(tmp[under_row][col]) > abs(tmp[col][col])) {
        max_row = under_row;
      }
    }
    if (max_row != col) {
      ans *= -1;
      std::swap(tmp[max_row], tmp[col]);
    }
    Field el = tmp[col][col];
    if (el == Field(0)) {
      return Field(0);
    }
    for (size_t under_row = col + 1; under_row < M; ++under_row) {
      Field num = tmp[under_row][col] / el;
      for (size_t i = col; i < M; ++i) {
        tmp[under_row][i] -= num * tmp[col][i];
      }
    }
    ans *= el;
  }
  return ans;
}

template <size_t M, size_t N, typename Field>
size_t Matrix<M, N, Field>::rank() const {
  size_t ans = 0;
  Matrix<M, N, Field> tmp(*this);
  for (size_t col = 0; col < M; ++col) {
    size_t max_row = col;
    for (size_t under_row = col + 1; under_row < M; ++under_row) {
      if (abs(tmp[under_row][col]) > abs(tmp[col][col])) {
        max_row = under_row;
      }
    }
    if (max_row != col) {
      std::swap(tmp[max_row], tmp[col]);
    }
    Field el = tmp[col][col];
    for (size_t under_row = col + 1; under_row < M; ++under_row) {
      Field num = tmp[under_row][col] / el;
      for (size_t i = 0; i < M; ++i) {
        tmp[under_row][i] -= num * tmp[col][i];
      }
    }
  }
  for (size_t i = 0; i < M; ++i) {
    size_t cnt = 0;
    for (size_t j = 0; j < N; ++j) {
      if (tmp[i][j] == Field(0)) {
        ++cnt;
      }
    }
    if (cnt != N) {
      ++ans;
    }
  }
  return ans;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::inverted() const {
  static_assert(M == N);
  Matrix<M, 2 * N, Field> tmp;
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < M; ++j) {
      tmp[i][j] = data_[i][j];
    }
    tmp[i][N + i] = Field(1);
  }

  for (size_t col = 0; col < M; ++col) {
    Field el = tmp[col][col];
    for (size_t under_row = col + 1; under_row < M; ++under_row) {
      Field num = tmp[under_row][col] / el;
      for (size_t i = col; i < 2 * M; ++i) {
        tmp[under_row][i] -= num * tmp[col][i];
      }
    }
  }
  for (int64_t k = M - 1; k >= 0; --k) {
    Field el = tmp[k][k];
    for (int64_t under_row = k - 1; under_row >= 0; --under_row) {
      Field num = tmp[under_row][k] / el;
      for (size_t i = k; i < 2 * M; ++i) {
        tmp[under_row][i] -= num * tmp[k][i];
      }
    }
  }
  Matrix<M, N, Field> ans;
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      ans[i][j] = tmp[i][N + j] / tmp[i][i];
    }
  }
  return ans;
}

template <size_t M, size_t N, typename Field>
void Matrix<M, N, Field>::invert() {
  this->data_ = inverted().data_;
}

template <size_t M, size_t N, typename Field>
Field Matrix<M, N, Field>::trace() const {
  static_assert(M == N);
  Field tr = 0;
  for (size_t i = 0; i < N; ++i) {
    tr += data_[i][i];
  }
  return tr;
}

template <size_t M, size_t N, typename Field>
std::array<Field, N> Matrix<M, N, Field>::getRow(size_t idx) const {
  return data_[idx];
}

template <size_t M, size_t N, typename Field>
std::array<Field, M> Matrix<M, N, Field>::getColumn(size_t idx) const {
  std::array<Field, M> col;
  for (size_t i = 0; i < M; ++i) {
    col[i] = data_[i][idx];
  }
  return col;
}

template <size_t M, size_t N, typename Field>
std::array<Field, N> &Matrix<M, N, Field>::operator[](size_t idx) {
  return data_[idx];
}

template <size_t M, size_t N, typename Field>
const std::array<Field, N> &Matrix<M, N, Field>::operator[](size_t idx) const {
  return data_[idx];
}

template <size_t M1, size_t N1, size_t M2, size_t N2, typename Field>
bool operator==(const Matrix<M1, N1, Field> &m1,
                const Matrix<M2, N2, Field> &m2) {
  if (M1 != M2 || N1 != N2) {
    return false;
  }
  for (size_t i = 0; i < M1; ++i) {
    for (size_t j = 0; j < N1; ++j) {
      if (m1[i][j] != m2[i][j]) {
        return false;
      }
    }
  }
  return true;
}

template <size_t M, size_t N, typename Field>
std::ostream &operator<<(std::ostream &os, const Matrix<M, N, Field> &m) {
  os << '{' << '\n';
  for (size_t i = 0; i < M; ++i) {
    os << '{';
    for (size_t j = 0; j < N; ++j) {
      os << m[i][j] << '\t';
    }
    os << '}' << '\n';
  }
  os << '}' << '\n';
  return os;
}
template <size_t M, size_t N, typename Field>
std::istream &operator>>(std::istream &is, Matrix<M, N, Field> &m) {
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      is >> m[i][j];
    }
  }

  return is;
}

template <size_t N, typename Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;
