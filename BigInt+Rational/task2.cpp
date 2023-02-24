#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>

#include "biginteger.h"

std::mt19937_64 rnd(
    std::chrono::steady_clock::now().time_since_epoch().count());

void test() {
  size_t count = 10000;
  for (size_t i = 0; i < count; ++i) {
    int64_t first = rnd() % 10000000;
    int64_t second = rnd() % 1000000000;
    first *= (rand() % 2 == 0 ? -1 : 1);
    second *= (rand() % 2 == 0 ? -1 : 1);

    BigInteger a(first);
    BigInteger b(second);
    assert((first + second) == (a + b));
    assert((-first + second) == (-a + b));
    assert((first + (-second)) == (a + (-b)));
    assert((first - second) == (a - b));
    assert((-first - second) == (-a - b));
    assert((first < 0) == (a < 0));
    assert((first > 0) == (a > 0));
    assert((first >= 0) == (a >= 0));
    assert((first <= 0) == (a <= 0));
    assert((first > second) == (a > b));
    assert((first >= second) == (a >= b));
    assert((first == second) == (a == b));
    assert((first < second) == (a < b));
    assert((first <= second) == (a <= b));
    assert((first * second) == (a * b));
    assert((first / second) == (a / b));
    assert((first % second) == (a % b));
  }

  std::cout << "BigInt passed\n";

  for (size_t i = 0; i < count; ++i) {
    int64_t first = rnd() % 1000000;
    int64_t second = rnd() % 1000000000;
    int64_t third = rnd() % 1000000000;
    int64_t fourth = rnd() % 100000;
    first *= -1;  //(rnd() % 2 == 0 ? -1 : 1);
    second *= (rnd() % 2 == 0 ? -1 : 1);
    third *= (rnd() % 2 == 0 ? -1 : 1);
    fourth *= -1;  //(rnd() % 2 == 0 ? -1 : 1);
    Rational a(first);
    a /= second;
    Rational b(third);
    b /= fourth;
    if (second * fourth != 0) {
      assert((a - b) ==
             Rational(first * fourth - third * second) / (second * fourth));
      assert(a + b ==
             Rational(first * fourth + third * second) / (second * fourth));
      assert(a * b == Rational(first * third) / (second * fourth));
      if (second * third != 0) {
        assert(a / b == Rational(first * fourth) / (second * third));
      }
    }
  }

  std::cout << "Rational passed\n";
}

int main() {
  /*BigInteger a("9000");
  BigInteger b("5000");
  BigInteger c("4");
  BigInteger d("-5");
  BigInteger e("7");
  BigInteger f("505");
  BigInteger p("-25000");

  assert(a + b == BigInteger("14000"));
  assert(a + c == BigInteger("9004"));
  assert(a + d == BigInteger("8995"));
  assert(a + d == BigInteger("8995"));

  assert(b - a == BigInteger("-4000"));
  assert(a - c == BigInteger("8996"));
  assert(a - d == BigInteger("9005"));
  assert(d - e == BigInteger("-12"));
  assert(c - e == BigInteger("-3"));

  assert(-a == BigInteger("-9000"));
  assert(-d == BigInteger("5"));

  assert(!(c > e));
  assert(b > c);

  assert(c * e == BigInteger("28"));
  assert(b * d == BigInteger("-25000"));
  assert(b * e == BigInteger("35000"));
  assert(c * e == BigInteger("28"));
  assert(f * d == BigInteger("-2525"));

  BigInteger x;
  BigInteger y;
  Rational s;
  Rational t = -402365939;
  ((s = 1000000007) *= 1000000009) *= 2147483647;
  std::cout << (t / s).asDecimal(25);
  */
  int64_t first = rnd() % 1000000000;
  int64_t second = rnd() % 1000000000;
  int64_t third = rnd() % 1000000000;
  int64_t fourth = rnd() % 1000000000;
  first *= (rnd() % 2 == 0 ? -1 : 1);
  second *= (rnd() % 2 == 0 ? -1 : 1);
  third *= (rnd() % 2 == 0 ? -1 : 1);
  fourth *= (rnd() % 2 == 0 ? -1 : 1);
  Rational a(first);
  a /= second;
  Rational b(third);
  b /= fourth;

  int64_t fifth = -999;
  int64_t sixth = 1000;
  Rational c(fifth);
  c /= sixth;
  int64_t seventh = 52151212;
  int64_t eigth = 3214;
  Rational d(seventh);
  d /= eigth;

  std::cout << a.toString() << '\n';
  std::cout << a.asDecimal(2) << '\n';
  std::cout << a.asDecimal(3) << '\n';
  std::cout << double(a) << '\n';
  std::cout << b.toString() << '\n';
  std::cout << b.asDecimal(5) << '\n';
  std::cout << b.asDecimal(10) << '\n';
  std::cout << double(b) << '\n';
  std::cout << c.toString() << '\n';
  std::cout << c.asDecimal(2) << '\n';
  std::cout << c.asDecimal(3) << '\n';
  std::cout << double(c) << '\n';
  std::cout << d.toString() << '\n';
  std::cout << d.asDecimal(2) << '\n';
  std::cout << d.asDecimal(3) << '\n';
  std::cout << double(d) << '\n';

  std::cout << (c < d) << '\n';
  std::cout << (c <= d) << '\n';
  std::cout << (c > d) << '\n';
  std::cout << (c >= d) << '\n';
  std::cout << (c == d) << '\n';
  std::cout << (c != d) << '\n';

  test();

  std::string s1 = "";
  std::string s2 = "";
  for (int i = 0; i < 30000; ++i) {
    s1 += std::to_string(rnd() % 10);
  }
  for (int i = 0; i < 5000; ++i) {
    s2 += std::to_string(rnd() % 10);
  }
  UnsignedBigInteger r = s1;
  UnsignedBigInteger l = s2;

  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();
  r /= l;
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "("
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     begin)
                   .count()
            << ")\n";
}
