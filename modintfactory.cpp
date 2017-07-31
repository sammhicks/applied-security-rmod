#include "modintfactory.hpp"

ModIntFactory::ModIntFactory(const BigInt &modulus) : mod(modulus) {
  mod.trim();

  range += 1;
  range <<= BigInt::Limbs(mod.size());

  conversion_factor = range * range;

  BigInt dummy;

  BigInt::div_mod(conversion_factor, mod, dummy);
}

ModInt ModIntFactory::create_int(const BigInt &value) {
  return ModInt(value * conversion_factor, *this);
}
