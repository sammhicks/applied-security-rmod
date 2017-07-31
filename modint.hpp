#pragma once

#include "bigint.hpp"

class ModInt;

#include "modintfactory.hpp"

class ModInt {
private:
  BigInt value;
  const ModIntFactory *const factory;

  ModInt(BigInt value, const ModIntFactory *const factory);

  static void reduce(BigInt &value, const ModIntFactory &factory);

  void reduce();

public:
  operator BigInt() const;

  friend ModInt operator*(const ModInt &a, const ModInt &b);

  friend class ModIntFactory;
};
