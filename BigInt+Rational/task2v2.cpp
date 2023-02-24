#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>

#include "biginteger.h"

std::mt19937_64 rnd(
    std::chrono::steady_clock::now().time_since_epoch().count());
int main() {
  UnsignedBigInteger ubi1;
  ubi1 = "123049841"_ubi;
  UnsignedBigInteger ubi2 = 156412379854;
  ubi2 += ubi2;

  std::cout << ubi1 << " + " << ubi2 << '\n';
  std::cout << (ubi1 + ubi2) << '\n';
  std::cout << ubi2 << " - " << ubi1 << '\n';
  std::cout << (ubi2 - ubi1) << '\n';
  std::cout << ubi1 << " * " << ubi2 << '\n';
  std::cout << (ubi1 * ubi2) << '\n';
  std::cout << ubi2 << " * " << ubi1 << '\n';
  std::cout << (ubi2 * ubi1) << '\n';
  std::cout << ubi1 << " / " << ubi2 << '\n';
  std::cout << (ubi1 / ubi2) << '\n';
  std::cout << ubi2 << " % " << ubi1 << '\n';
  std::cout << (ubi2 % ubi1) << '\n';

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
  //   UnsignedBigInteger bbb = r;
  /*for (int i = 0; i < 5000 / 9; ++i) {
    int64_t first = rnd() % 1000000000;
    r += l.times(first).shift_chunk(i);
  }*/
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "("
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     begin)
                   .count()
            << ")\n";
}