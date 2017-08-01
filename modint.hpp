#pragma once

#include "bigint.hpp"

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

  static bool sliding_window_k_check(ptrdiff_t log_n, ptrdiff_t k);

  static ptrdiff_t power_to_array_index(ptrdiff_t p);

public:
  operator BigInt() const;

  ModInt create_from_same_factory(const BigInt &value) const;

  friend ModInt operator*(const ModInt &a, const ModInt &b);

  // calculate x^n
  static ModInt pow(const ModInt &x, const BigInt &n);

  friend class ModIntFactory;
};
