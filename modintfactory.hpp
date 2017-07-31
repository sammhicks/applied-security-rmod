#pragma once

#include "bigint.hpp"

class ModIntFactory;

#include "modint.hpp"

class ModIntFactory {
private:
  BigInt mod;
  BigInt neg_inv_mod;
  BigInt range;
  BigInt conversion_factor;

public:
  ModIntFactory(BigInt mod);

  friend class ModInt;
};
