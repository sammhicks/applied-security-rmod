
#include <cstdlib>
#include <iostream>

#include "bigint.hpp"

using std::cout;
using std::endl;

int main() {
  BigInt a, b;

  for (long n = 0; n < 13; ++n) {
    a += 1;
  }

  for (int n = 0; n < 7; ++n) {
    b += 1;
  }

  cout << "A:" << a << endl;
  cout << "B:" << b << endl;

  BigInt::limb_t c = a / b;

  cout << "C = A / B:" << c << endl;
  cout << "A % B:" << a << endl;

  return EXIT_SUCCESS;
}
