#pragma once

#include <algorithm>

#include "bigint.hpp"

using std::out_of_range;
using std::runtime_error;

class ModInt;

#include "modintfactory.hpp"

class ModInt {
private:
  BigInt value;
  const ModIntFactory *factory;

  ModInt() = default;
  ModInt(BigInt value, const ModIntFactory *factory);

  static void reduce(BigInt &value, const ModIntFactory &factory);

  void reduce();

  static bool sliding_window_k_check(BigInt::bit_index_type log_n,
                                     BigInt::bit_index_type k);

  static ptrdiff_t power_to_array_index(ptrdiff_t p, ptrdiff_t count);

public:
  explicit operator BigInt() const;

  ModInt create_from_same_factory(const BigInt &value) const;

  ModInt &operator+=(const ModInt &rhs);
  ModInt &operator-=(const ModInt &rhs);

  friend ModInt operator*(const ModInt &a, const ModInt &b);

  // calculate x^n
  ModInt pow(const BigInt &n) const;
  static ModInt pow(const ModInt &x, const BigInt &n);

  friend class ModIntFactory;
};

ModInt operator+(const ModInt &lhs, const ModInt &rhs);
ModInt operator-(const ModInt &lhs, const ModInt &rhs);
ModInt operator%(const ModInt &mod_value, const ModIntFactory &factory);
