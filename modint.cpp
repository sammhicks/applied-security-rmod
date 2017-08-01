#include "modint.hpp"

ModInt::ModInt(BigInt value, const ModIntFactory *factory)
    : value(value), factory(factory) {
  reduce();
}

ModInt ModInt::create_from_same_factory(const BigInt &value) const {
  return factory->create_int(value);
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

bool ModInt::sliding_window_k_check(ptrdiff_t log_n, ptrdiff_t k) {
  return log_n < (k * (k + 1) * (1 << (2 * k))) / ((1 << (k + 1)) - k - 2) + 1;
}

ptrdiff_t power_to_array_index(ptrdiff_t p) { return (p - 1) / 2; }

ModInt ModInt::pow(const ModInt &x, const BigInt &n) {
  std::cout << "Pow" << std::endl;
  ptrdiff_t log_n = n.size() * BigInt::LIMB_WIDTH;

  ptrdiff_t k = 1;

  while (!sliding_window_k_check(log_n, k)) {
    ++k;
  }

  std::cout << "k: " << k << std::endl;

  ptrdiff_t precalculated_items_count = 1 << (k - 1);

  ModInt precalculated_items[precalculated_items_count];

  ModInt x_squared = x * x;

  precalculated_items[0] = x;

  for (ptrdiff_t i = 1; i < precalculated_items_count; ++i) {
    precalculated_items[i] = precalculated_items[i - 1] * x_squared;
  }

  ModInt result = x.create_from_same_factory(BigInt(1));

  return result;
}
