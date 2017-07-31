
#include <cstdlib>
#include <iostream>

#include "bigint.hpp"
#include "modint.hpp"

using std::cin;
using std::cout;
using std::endl;

int main() {
  BigInt a, b, n;

  cin >> a >> b >> n;

  cout << "A:" << a << endl;
  cout << "B:" << b << endl;
  cout << "N:" << n << endl;

  ModIntFactory factory(n);

  ModInt a_mod = factory.create_int(a);
  ModInt b_mod = factory.create_int(b);

  cout << "A * B = " << static_cast<BigInt>(a_mod * b_mod) << endl;

  return EXIT_SUCCESS;
}
