
#include <cstdlib>
#include <iostream>

#include "bigint.hpp"

using std::cin;
using std::cout;
using std::endl;

int main() {
  BigInt a;
  BigInt b;

  cin >> a >> b;

  cout << "A:" << a << endl;
  cout << "B:" << b << endl;

  BigInt a_div_b, a_mod_b;

  BigInt::div_mod(a, b, a_div_b, a_mod_b);

  cout << "A / B:" << a_div_b << endl;
  cout << "A % B:" << a_mod_b << endl;

  return EXIT_SUCCESS;
}
