
#include <cstdlib>
#include <iostream>

#include "bigint.hpp"

using std::cout;
using std::endl;

int main() {
  BigInt a, b;

  for (int n = 0; n < 17; ++n) {
    a += 1;
  }

  for (int n = 0; n < 14; ++n) {
    b += 1;
  }

  cout << "A:" << a << endl;
  cout << "B:" << b << endl;

  BigInt c;

  c += a;
  c -= b;

  cout << "C = A - B:" << c << endl;

  return EXIT_SUCCESS;
}
