
#include <cstdlib>
#include <iostream>

#include "bigint.hpp"

using std::cin;
using std::cout;
using std::endl;

int main() {
  BigInt b, n;

  cin >> b >> n;

  cout << "B:" << b << endl;
  cout << "N:" << n << endl;

  cout << "Inverse of B mod N: " << BigInt::mod_inv(b, n) << endl;

  return EXIT_SUCCESS;
}
