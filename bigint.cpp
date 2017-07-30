#include "bigint.hpp"

const BigInt::double_limb_t BigInt::LIMB_WIDTH = 4;
const BigInt::double_limb_t BigInt::LIMB_MODULUS = 1 << BigInt::LIMB_WIDTH;
const BigInt::double_limb_t BigInt::LIMB_MASK = BigInt::LIMB_MODULUS - 1;

void BigInt::maybe_add_leading_zero(limbs_t &limbs, limbs_iter_t &iter) {
  if (iter == limbs.end()) {
    limbs.push_back(0);
    iter = limbs.end();
    --iter;
  }
}

void BigInt::split_double_limb(double_limb_t d, double_limb_t &large,
                               limb_t &small) {
  small = d & LIMB_MASK;
  large = (d >> LIMB_WIDTH) & LIMB_MASK;
}

void BigInt::add_limb(limbs_t &lhs_limbs, limbs_iter_t lhs_iter,
                      double_limb_t rhs) {
  maybe_add_leading_zero(lhs_limbs, lhs_iter);

  double_limb_t carry = 0;

  split_double_limb(rhs + *lhs_iter, carry, *lhs_iter);

  ++lhs_iter;

  if (carry > 0) {
    add_limb(lhs_limbs, lhs_iter, carry);
  }
}

void BigInt::add_big_int(limbs_t &lhs_limbs, limbs_iter_t lhs_iter,
                         const limbs_t &rhs_limbs,
                         limbs_const_iter_t rhs_iter) {
  while (rhs_iter != rhs_limbs.cend()) {
    add_limb(lhs_limbs, lhs_iter, *rhs_iter);

    ++lhs_iter;
    ++rhs_iter;
  }
}

BigInt &BigInt::operator+=(limb_t rhs) {
  add_limb(limbs, limbs.begin(), rhs);

  return *this;
}

BigInt &BigInt::operator+=(const BigInt &rhs) {
  add_big_int(limbs, limbs.begin(), rhs.limbs, rhs.limbs.cbegin());

  return *this;
}

void BigInt::subtract_limb(limbs_t &lhs_limbs, limbs_iter_t lhs_iter,
                           limb_t rhs) {
  if (lhs_iter == lhs_limbs.end()) {
    throw underflow_error("Negative number occurred when subtracting");
  } else if (rhs <= *lhs_iter) {
    *lhs_iter -= rhs;
  } else {
    *lhs_iter += LIMB_MODULUS - rhs;

    ++lhs_iter;

    subtract_limb(lhs_limbs, lhs_iter, 1);
  }
}

void BigInt::subtract_big_int(limbs_t &lhs_limbs, limbs_iter_t lhs_iter,
                              const limbs_t &rhs_limbs,
                              limbs_const_iter_t rhs_iter) {
  while (rhs_iter != rhs_limbs.cend()) {
    subtract_limb(lhs_limbs, lhs_iter, *rhs_iter);

    ++lhs_iter;
    ++rhs_iter;
  }
}

BigInt &BigInt::operator-=(limb_t rhs) {
  subtract_limb(limbs, limbs.begin(), rhs);

  return *this;
}

BigInt &BigInt::operator-=(const BigInt &rhs) {
  subtract_big_int(limbs, limbs.begin(), rhs.limbs, rhs.limbs.cbegin());

  return *this;
}

void BigInt::multiply_by_limb(limbs_t &acc_limbs, limbs_iter_t acc_iter,
                              const limbs_t &lhs_limbs,
                              limbs_const_iter_t lhs_iter, limb_t rhs) {
  while (lhs_iter != lhs_limbs.cend()) {
    add_limb(acc_limbs, acc_iter, static_cast<double_limb_t>(rhs) * *lhs_iter);

    ++lhs_iter;
    ++acc_iter;
  }
}

void BigInt::multiply_by_big_int(limbs_t &acc_limbs, limbs_iter_t acc_iter,
                                 const limbs_t &lhs_limbs,
                                 limbs_const_iter_t lhs_iter,
                                 const limbs_t &rhs_limbs,
                                 limbs_const_iter_t rhs_iter) {
  while (rhs_iter != rhs_limbs.cend()) {
    multiply_by_limb(acc_limbs, acc_iter, lhs_limbs, lhs_iter, *rhs_iter);

    ++acc_iter;
    ++rhs_iter;
  }
}

BigInt BigInt::operator*(limb_t rhs) {
  BigInt result;
  multiply_by_limb(result.limbs, result.limbs.begin(), limbs, limbs.cbegin(),
                   rhs);
  return result;
}

BigInt BigInt::operator*(const BigInt &rhs) {
  BigInt result;
  multiply_by_big_int(result.limbs, result.limbs.begin(), limbs, limbs.cbegin(),
                      rhs.limbs, rhs.limbs.cbegin());
  return result;
}

ostream &operator<<(ostream &os, const BigInt &value) {
  ios::fmtflags f(os.flags());

  os << std::hex;

  os << "BigInt:";
  for (auto it = value.limbs.rbegin(); it != value.limbs.rend(); ++it) {
    os << *it;
  }

  os.flags(f);

  return os;
}
