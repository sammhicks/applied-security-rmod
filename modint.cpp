#include "modint.hpp"

ModInt::ModInt(BigInt value, const ModIntFactory *factory)
    : value(value), factory(factory) {
  reduce();
}

ModInt ModInt::create_from_same_factory(const BigInt &value) const {
  // std::cout << "Creating from " << value << std::endl;
  // std::cout << "Factory " << factory << std::endl;
  return factory->create_int(value);
}

void ModInt::reduce(BigInt &value, const ModIntFactory &factory) {
  value.trim();

  /*std::cout << "Reducing " << value << " with factory " << &factory
            << std::endl;*/

  // std::cout << "Mod0: ";
  BigInt::limb_t mod0 = factory.mod.least_significant_limb();
  // std::cout << mod0 << std::endl;

  // std::cout << "Inv Mod0: ";
  BigInt::limb_t inv_mod0 = BigInt::mod_inv(mod0, BigInt::LIMB_MODULUS);
  // std::cout << inv_mod0 << std::endl;

  // std::cout << "Neg Inv Mod0: ";
  BigInt::limb_t neg_inv_mod0 = BigInt::LIMB_MODULUS - inv_mod0;
  // std::cout << neg_inv_mod0 << std::endl;

  // std::cout << "Mod size: ";
  size_t mod_size = factory.mod.size();
  // std::cout << mod_size << std::endl;

  for (size_t n = 0; n < mod_size; ++n) {
    // std::cout << "Least significant limb" << std::endl;
    // std::cout << value.least_significant_limb() << std::endl;

    BigInt::limb_t k = (value.least_significant_limb() * neg_inv_mod0);

    k &= BigInt::LIMB_MASK;

    // std::cout << "k " << k << std::endl;

    value += factory.mod * k;
    value >>= BigInt::Limbs(1);
  }

  // std::cout << "Done reducing!" << std::endl;
}

void ModInt::reduce() { reduce(value, *factory); }

ModInt::operator BigInt() const {
  BigInt result = value;
  reduce(result, *factory);
  return result;
}

ModInt operator*(const ModInt &a, const ModInt &b) {
  // std::cout << "a: " << a.factory << std::endl;
  // std::cout << "b: " << b.factory << std::endl;
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
    // std::cout << "Lookup: " << p << "(@" << i << ") of " << count <<
    // std::endl;

    return i;
  } else {
    throw out_of_range(
        "Sliding window precalculated item array lookup out of range!");
  }
}

ModInt ModInt::pow(const BigInt &n) const { return ModInt::pow(*this, n); }

// https://en.wikipedia.org/wiki/Exponentiation_by_squaring#Sliding_window_method
ModInt ModInt::pow(const ModInt &x, const BigInt &n) {
  // std::cout << "Pow" << std::endl;
  ptrdiff_t log_n = n.size() * BigInt::LIMB_WIDTH;

  // std::cout << "log_n: " << log_n << std::endl;

  ptrdiff_t k = 1;

  while (!sliding_window_k_check(log_n, k)) {
    ++k;
  }

  std::cout << "k: " << k << std::endl;

  ptrdiff_t precalculated_items_count = 1 << k;

  ModInt precalculated_items[precalculated_items_count];

  ModInt x_squared = x * x;

  precalculated_items[0] = x;

  for (ptrdiff_t i = 1; i < precalculated_items_count; ++i) {
    precalculated_items[i] = precalculated_items[i - 1] * x_squared;
  }

  /*std::cout << "Precalculated Items:" << std::endl;

  for (ptrdiff_t i = 0; i < precalculated_items_count; ++i) {
    std::cout << "\t" << static_cast<BigInt>(precalculated_items[i])
              << std::endl;
  }*/

  // 1.  y := 1; i := l-1
  ModInt result = x.create_from_same_factory(BigInt(1));
  BitIterator i(n);

  // 2.  while i > -1 do
  while (!i.is_finished()) {
    std::cout << "i: " << *i << std::endl;
    // 3.      if ni=0 then y:=y2' i:=i-1
    if (!*i) {
      std::cout << std::endl << "bit: 0" << std::endl << std::endl;

      result = result * result;
      std::cout << "i:";
      ++i;
    }
    // 4.      else
    else {
      std::cout << std::endl << "bit: 1" << std::endl << std::endl;
      i.clear_acc();
      BitIterator s = i;

      ptrdiff_t i_s_difference = 0;

      std::cout << "s looks ahead" << std::endl;
      // 5.          s:=max{i-k+1,0}
      while (!s.is_finished() && i_s_difference < k) {
        std::cout << "s:";
        ++s;
        ++i_s_difference;
      }

      std::cout << "backtracking" << std::endl;

      // 6.          while ns=0 do s:=s+1 [notes 1]
      while (!*s) {
        std::cout << "s:";
        --s;
      }
      // 7.          for h:=1 to i-s+1 do y:=y2
      while (i != s) {
        std::cout << "i:";
        ++i;
        result = result * result;
      }
      // 8.          u:=(ni,ni-1,....,ns)2
      // 9.          y:=y*xu
      std::cout << std::endl
                << "bits: " << std::bitset<4>(i.get_acc()) << std::endl
                << std::endl;
      result = result * precalculated_items[power_to_array_index(
                            i.get_acc(), precalculated_items_count)];
      // 10.         i:=s-1
      std::cout << "i:";
      ++i;

      std::cout << "end of 1 block" << std::endl;
    }
  }

  std::cout << "Done!" << std::endl;

  // 11. return y
  return result;
}
