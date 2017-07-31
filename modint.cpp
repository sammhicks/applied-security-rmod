#include "modint.hpp"

ModInt::ModInt(BigInt value, const ModIntFactory *const factory)
    : value(value), factory(factory) {
  reduce();
}

void ModInt::reduce(BigInt &value, const ModIntFactory &factory) {
  BigInt::limb_t mod0 = factory.mod.least_significant_limb();

  BigInt::limb_t inv_mod0 = BigInt::mod_inv(mod0, BigInt::LIMB_MODULUS);

  BigInt::limb_t neg_inv_mod0 = BigInt::LIMB_MODULUS - inv_mod0;

  size_t mod_size = factory.mod.size();

  for (size_t n = 0; n < mod_size; ++n) {
    BigInt::limb_t k = (value.least_significant_limb() * neg_inv_mod0);

    k &= BigInt::LIMB_MASK;

    value += factory.mod * k;
    value >>= BigInt::Limbs(1);
  }
}

void ModInt::reduce() { reduce(value, *factory); }

ModInt::operator BigInt() const {
  BigInt result = value;
  reduce(result, *factory);
  return result;
}

ModInt operator*(const ModInt &a, const ModInt &b) {
  if (a.factory == b.factory) {
    return ModInt(a.value * b.value, a.factory);
  } else {
    throw domain_error("Montgomery multiplication must "
                       "be over the same modulus!");
  }
}
