
#include <cstdlib>
#include <iostream>

#include "bigint.hpp"
#include "modint.hpp"

using std::cin;
using std::cout;
using std::endl;

int main() {

  // BigInt a, b, n;

  BigInt a("123456");
  BigInt b("876543");
  BigInt n("3a8f05c5");

  // cin >> a >> b >> n;

  cout << "A:" << a << endl;
  cout << "B:" << b << endl;
  cout << "N:" << n << endl;

  ModIntFactory factory(n);

  ModInt a_mod = factory.create_int(a);

  cout << "A ^ B = " << static_cast<BigInt>(a_mod.pow(b)) << endl;

  /*BigInt a("ac0");

  ModIntFactory factory(BigInt("9887"));

  ModInt a_mod = factory.create_int(a);

  cout << static_cast<BigInt>(a_mod * a_mod) << std::endl;*/

  return EXIT_SUCCESS;
}
