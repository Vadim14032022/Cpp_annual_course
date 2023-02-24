#include <cstring>
#include <iostream>
#include <sstream>
#include <typeinfo>

#include "string.h"
int main() {
  try {
    std::stringstream ss;
    const String s1 = "abcabcd";
    String s2 = s1.substr(2, 10);
    std::cout << s2;
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}
