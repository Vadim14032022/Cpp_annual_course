#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include "matrix.h"

int main() {
  /*{
    Rational r;
    // std::cin >> r;
    std::cout << r << '\n';
    Residue<17> res(-345);
    std::cout << res / Residue<17>(8) << '\n';
  }
  {
    // Matrix<6, 8> m1;
    // Matrix<6, 8> m2;
    // Matrix<8, 6> m3;
    std::initializer_list<std::initializer_list<Residue<17>>> ab = {
        {1, 2, 3, 4}, {2, 3, 4, 5}, {3, 4, 5, 6}, {4, 5, 6, 7}};
    Matrix<4, 4, Residue<17>> abm = {
        {1, 2, 3, 4}, {2, 3, 4, 5}, {3, 4, 5, 6}, {4, 5, 6, 7}};
    // assert((m1 == m2));
    // assert((!(m1 == m3)));
    assert((abm == SquareMatrix<4, Residue<17>>(ab)));

    Matrix<4, 4> m;
    // std::cin >> m;
    // assert(m.det() == Rational(-1));
  }
  {
    Matrix<4, 5, Residue<17>> am = {
        {1, 2, 3, 4, 5}, {2, 3, 4, 5, 6}, {3, 4, 5, 6, 7}, {4, 5, 6, 7, 8}};
    Matrix<5, 4, Residue<17>> bm = {
        {3, 4, 5, 6}, {4, 5, 6, 7}, {1, 2, 3, 4}, {2, 3, 4, 5}, {5, 6, 7, 8}};
    std::initializer_list<std::initializer_list<Residue<17>>> ab = {
        {1, 2, 3, 4}, {2, 3, 4, 5}, {3, 4, 5, 6}, {4, 5, 6, 7}};
    SquareMatrix<4, Residue<17>> abm = am * bm;
    std::cout << abm;
    am += am;
    std::cout << am;
    am *= 3;
    std::cout << am;
    // am -= bm;
    std::cout << am;
    // am *= bm;
    std::cout << am;
    std::cout << am.transposed();
  }*/
  {
    Rational a(1);
    a /= 10;
    Rational b(19);
    b /= 100;
    std::cout << (a == b);
    Matrix<5, 5> m1 = {{46, 56, 27, 32, 48},
                       {19, 55, 57, 69, 43},
                       {11, 93, 47, 47, 29},
                       {14, 41, 75, 53, 55},
                       {96, 10, 43, 12, 50}};
    SquareMatrix<4> m2 = {
        {79, 26, 73, 14}, {26, 23, 88, 35}, {95, 45, 79, 90}, {94, 11, 46, 68}};
    SquareMatrix<3> m3 = {{0, 0, 1}, {0, 1, 0}, {1, 0, 0}};
    std::cout << m1 << '\n';
    std::cout << m1.det() << '\n';
    std::cout << m2 << '\n';
    std::cout << m2.det() << '\n';
    std::cout << m3 << '\n';
    std::cout << m3.det() << '\n';
  }
  std::ifstream file;
  file.open("matr.txt");
  SquareMatrix<20> m;
  file >> m;

  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();
  std::cout << m.inverted();
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "("
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     begin)
                   .count()
            << ")\n";

  file.close();
}