#pragma once

#include "bigint.hpp"

class ModInt;

#include "modintfactory.hpp"

class ModInt {
private:
  BigInt value;

  const ModIntFactory &factory;

  void reduce();

public:
  ModInt(BigInt value, const ModIntFactory &factory);

  friend class ModIntFactory;
};
