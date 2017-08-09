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

  BigInt::limb_type mod0 = factory.mod.least_significant_limb();

  BigInt::limb_type inv_mod0 = BigInt::mod_inv(mod0, BigInt::LIMB_MODULUS);

  BigInt::limb_type neg_inv_mod0 = BigInt::LIMB_MODULUS - inv_mod0;

  BigInt::limbs_size_type limb_count = factory.mod.limb_count();

  for (BigInt::limbs_size_type n = 0; n < limb_count; ++n) {
    BigInt::limb_type k = (value.least_significant_limb() * neg_inv_mod0);

    k &= BigInt::LIMB_MASK;

    value += factory.mod * k;
    value >>= BigInt::Limbs(1);
  }

  if (value > factory.mod) {
    // std::cout << std::endl << "REDUCTION!!!" << std::endl << std::endl;
    value %= factory.mod;
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
bool ModInt::sliding_window_k_check(BigInt::bit_index_type log_n,
                                    BigInt::bit_index_type k) {
  return log_n <
         ((k * (k + 1) * (1 << (2 * k))) / ((1 << (k + 1)) - k - 2) + 1);
}

ptrdiff_t ModInt::power_to_array_index(ptrdiff_t p, ptrdiff_t count) {
  // std::cout << "Power: " << p << std::endl;
  ptrdiff_t i = (p - 1) / 2;
  // std::cout << "Index: " << i << std::endl;
  // std::cout << "Count: " << count << std::endl;
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
  BigInt::bit_index_type log_n = n.log_2();

  BigInt::bit_index_type k = 1;

  while (!sliding_window_k_check(log_n, k)) {
    ++k;
  }

  // std::cout << "k:" << k << std::endl;

  ptrdiff_t precalculated_items_count = 1 << k;

  // std::cout << "Precaclulated items count: " << precalculated_items_count
  //    << std::endl;

  ModInt precalculated_items[precalculated_items_count];

  ModInt x_squared = x * x;

  precalculated_items[0] = x;

  for (ptrdiff_t i = 1; i < precalculated_items_count; ++i) {
    precalculated_items[i] = precalculated_items[i - 1] * x_squared;
  }

  // 1.  y := 1; i := l-1
  ModInt y = x.create_from_same_factory(BigInt(1));

  BigInt::bit_index_type i = n.log_2();

  // 2.  while i > -1 do
  while (i > -1) {
    // std::cout << "i: " << i << std::endl;
    // 3.      if ni=0 then y:=y2' i:=i-1
    if (n[i] == 0) {
      // std::cout << "Zero" << std::endl;
      y = y * y;
      --i;
    }
    // 4.      else
    else {
      // 5.          s:=max{i-k+1,0}
      BigInt::bit_index_type s =
          std::max(i - k + 1, static_cast<BigInt::bit_index_type>(0));

      // 6.          while ns=0 do s:=s+1 [notes 1]
      while (n[s] == 0) {
        ++s;
      }
      // 7.          for h:=1 to i-s+1 do y:=y2
      for (BigInt::bit_index_type h = 1; h <= i - s + 1; ++h) {
        y = y * y;
      }
      // 8.          u:=(ni,ni-1,....,ns)2
      size_t u = 0;
      for (BigInt::bit_index_type h = i; h >= s; --h) {
        u = (u << 1) + n[h];
      }
      // std::cout << "u: " << u << std::endl;
      // 9.          y:=y*xu
      y = precalculated_items[power_to_array_index(u,
                                                   precalculated_items_count)] *
          y;
      // 10.         i:=s-1
      i = s - 1;
    }
  }
  // 11. return y
  return y;
}
