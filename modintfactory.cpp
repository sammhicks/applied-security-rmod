#include "modintfactory.hpp"

ModIntFactory::ModIntFactory(const BigInt &modulus) : mod(modulus) {
  mod.trim();

  BigInt range(1);
  range <<= BigInt::Limbs(mod.limb_count());

  conversion_factor = range * range;

  conversion_factor %= mod;
}

ModInt ModIntFactory::create_int(const BigInt &value) const {
  return ModInt(value * conversion_factor, this);
}

ModInt operator%(const BigInt &value, const ModIntFactory &factory) {
  return factory.create_int(value);
}
