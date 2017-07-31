#include "modint.hpp"

ModInt::ModInt(BigInt value, const ModIntFactory &factory)
    : value(value), factory(factory) {
  reduce();
}

void ModInt::reduce() {
  value.trim();

  BigInt::limb_t mod0 = factory.mod.least_significant_limb();

  BigInt::limb_t inv_mod0 = BigInt::mod_inv(mod0, BigInt::LIMB_MODULUS);

  BigInt::limb_t neg_inv_mod0 = BigInt::LIMB_MODULUS - inv_mod0;

  size_t value_size = value.size();

  for (size_t n = 0; n < value_size; ++n) {
    BigInt::limb_t k = (value.least_significant_limb() * neg_inv_mod0);

    k &= BigInt::LIMB_MASK;

    value += factory.mod * k;
    value >>= BigInt::Limbs(1);
  }
}
