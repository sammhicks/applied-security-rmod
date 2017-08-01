#include "modintfactory.hpp"

ModIntFactory::ModIntFactory(const BigInt &modulus) : mod(modulus) {
  mod.trim();

  BigInt range(1);
  range <<= BigInt::Limbs(mod.size());

  conversion_factor = range * range;

  conversion_factor %= mod;
}

ModInt ModIntFactory::create_int(const BigInt &value) const {
  return ModInt(value * conversion_factor, this);
}
