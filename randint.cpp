#include "randint.hpp"

void seed_generator() {
  unsigned int seed;

  ifstream dev_random("/dev/random");

  dev_random.read(reinterpret_cast<char *>(&seed), sizeof(seed));

  dev_random.close();

  srand(seed);
}

BigInt::limb_type random_limb() { return rand() & BigInt::LIMB_MASK; }

BigInt::limb_type random_limb(BigInt::limb_type range) {
  BigInt::limb_type range_mask = 1;

  while (range_mask < range) {
    range_mask <<= 1;
  }
  --range_mask;

  BigInt::limb_type limb;

  do {
    limb = random_limb() & range_mask;
  } while (limb >= range);

  return limb;
}

BigInt random_bigint(const BigInt &range) {
  if (range == 0) {
    throw range_error("RNG range cannot be zero");
  }

  BigInt result;

  BigInt::limbs_const_iter_type most_significant_limb =
      range.most_significant_limb();

  for (BigInt::limbs_const_iter_type limb_iter = range.least_significant_limb();
       limb_iter != most_significant_limb; ++limb_iter) {
    result.append_limb(random_limb());
  }

  result.append_limb(random_limb(*most_significant_limb));

  return result;
}

BigInt random_bigint(const BigInt &lower, const BigInt &upper) {
  BigInt range = upper;
  range -= lower;

  BigInt result = random_bigint(range);

  result += lower;

  return result;
}
