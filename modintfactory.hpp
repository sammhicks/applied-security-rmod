#pragma once

#include "bigint.hpp"

class ModIntFactory;

#include "modint.hpp"

class ModIntFactory {
private:
  BigInt mod;
  BigInt conversion_factor;

public:
  ModIntFactory(const BigInt &modulus);

  ModInt create_int(const BigInt &value) const;

  friend class ModInt;
};

ModInt operator%(const BigInt &value, const ModIntFactory &factory);
