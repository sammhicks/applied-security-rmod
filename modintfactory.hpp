#pragma once

#include "bigint.hpp"

class ModIntFactory;

#include "modint.hpp"

class ModIntFactory {
private:
  BigInt mod;
  BigInt range;
  BigInt conversion_factor;

public:
  ModIntFactory(const BigInt &modulus);

  ModInt create_int(const BigInt &value);

  friend class ModInt;
};
