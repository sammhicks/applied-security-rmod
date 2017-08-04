#include "modint.hpp"

ModInt::ModInt(BigInt value, const ModIntFactory *factory)
    : value(value), factory(factory) {
  reduce();
}

ModInt ModInt::create_from_same_factory(const BigInt &value) const {
  return factory->create_int(value);
}

void ModInt::reduce(BigInt &value, const ModIntFactory &factory) {
  value.trim();

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

// https://wikimedia.org/api/rest_v1/media/math/render/svg/1e865f7688532c911e9c0f65df83d8d3976b2ecc
bool ModInt::sliding_window_k_check(ptrdiff_t log_n, ptrdiff_t k) {
  return log_n <
         ((k * (k + 1) * (1 << (2 * k))) / ((1 << (k + 1)) - k - 2) + 1);
}

ptrdiff_t ModInt::power_to_array_index(ptrdiff_t p, ptrdiff_t count) {
  ptrdiff_t i = (p - 1) / 2;
  if ((i >= 0) && (i < count)) {
    return i;
  } else {
    throw out_of_range(
        "Sliding window precalculated item array lookup out of range!");
  }
}

ModInt ModInt::pow(const BigInt &n) const { return ModInt::pow(*this, n); }

// https://en.wikipedia.org/wiki/Exponentiation_by_squaring#Sliding_window_method
ModInt ModInt::pow(const ModInt &x, const BigInt &n) {
  ptrdiff_t log_n = n.size() * BigInt::LIMB_WIDTH;

  ptrdiff_t k = 1;

  while (!sliding_window_k_check(log_n, k)) {
    ++k;
  }

  ptrdiff_t precalculated_items_count = 1 << k;

  ModInt precalculated_items[precalculated_items_count];

  ModInt x_squared = x * x;

  precalculated_items[0] = x;

  for (ptrdiff_t i = 1; i < precalculated_items_count; ++i) {
    precalculated_items[i] = precalculated_items[i - 1] * x_squared;
  }

  // 1.  y := 1; i := l-1
  ModInt result = x.create_from_same_factory(BigInt(1));
  BitIterator i(n);

  // 2.  while i > -1 do
  while (!i.is_finished()) {
    // std::cout << "Current: " << static_cast<BigInt>(result) << std::endl;
    // 3.      if ni=0 then y:=y2' i:=i-1
    if (!*i) {
      // std::cout << "Zero" << std::endl;
      result = result * result;
      ++i;
    }
    // 4.      else
    else {
      // std::cout << "One" << std::endl;
      i.clear_acc();
      BitIterator s = i;

      ptrdiff_t digits_included = 1;

      // 5.          s:=max{i-k+1,0}
      while (!s.is_finished() && digits_included < k) {
        ++s;
        ++digits_included;
      }

      // 6.          while ns=0 do s:=s+1 [notes 1]
      while (!*s) {
        --s;
      }
      // 7.          for h:=1 to i-s+1 do y:=y2
      while (i != s) {
        ++i;
        result = result * result;
      }
      result = result * result;
      // 8.          u:=(ni,ni-1,....,ns)2
      // 9.          y:=y*xu
      result = result * precalculated_items[power_to_array_index(
                            i.get_acc(), precalculated_items_count)];
      // 10.         i:=s-1
      ++i;
    }
  }
  // 11. return y
  return result;
}
