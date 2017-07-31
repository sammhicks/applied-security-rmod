#include "bigint.hpp"

const BigInt::double_limb_t BigInt::LIMB_WIDTH = 1;
const BigInt::double_limb_t BigInt::LIMB_MODULUS = 1 << BigInt::LIMB_WIDTH;
const BigInt::double_limb_t BigInt::LIMB_MASK = BigInt::LIMB_MODULUS - 1;

void BigInt::maybe_add_leading_zero(limbs_t &limbs, limbs_iter_t &iter) {
  if (iter == limbs.end()) {
    limbs.push_back(0);
    iter = limbs.end();
    --iter;
  }
}

void BigInt::remove_leading_zeros(limbs_t &limbs) {
  while (!limbs.empty() && limbs.back() == 0) {
    limbs.pop_back();
  }
}

void BigInt::split_double_limb(double_limb_t d, double_limb_t &large,
                               limb_t &small) {
  small = d & LIMB_MASK;
  large = (d >> LIMB_WIDTH) & LIMB_MASK;
}

BigInt::limbs_const_iter_t BigInt::first_non_zero(limbs_const_iter_t start,
                                                  limbs_const_iter_t end) {
  limbs_const_iter_t iter = end;
  --iter;
  while (iter >= start && *iter == 0) {
    --iter;
  }

  return iter;
}

BigInt::Comparison BigInt::compare(limbs_const_iter_t lhs_start,
                                   limbs_const_iter_t lhs_end,
                                   limbs_const_iter_t rhs_start,
                                   limbs_const_iter_t rhs_end) {
  limbs_const_iter_t lhs_iter = first_non_zero(lhs_start, lhs_end);
  limbs_const_iter_t rhs_iter = first_non_zero(rhs_start, rhs_end);

  limbs_difference_t lhs_length = 1 + lhs_iter - lhs_start;
  limbs_difference_t rhs_length = 1 + rhs_iter - rhs_start;

  Comparison length_comparison =
      compare<limbs_difference_t>(lhs_length, rhs_length);

  if (length_comparison != Comparison::EQUALS) {
    return length_comparison;
  } else {
    while (lhs_iter >= lhs_start && rhs_iter >= rhs_start) {
      Comparison value_comparison = compare(*lhs_iter, *rhs_iter);

      if (value_comparison != Comparison::EQUALS) {
        return value_comparison;
      }

      --lhs_iter;
      --rhs_iter;
    }

    return Comparison::EQUALS;
  }
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
                         limbs_const_iter_t rhs_iter,
                         limbs_const_iter_t rhs_end) {
  while (rhs_iter != rhs_end) {
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
  add_big_int(limbs, limbs.begin(), rhs.limbs.cbegin(), rhs.limbs.cend());

  return *this;
}

void BigInt::subtract_limb(limbs_iter_t lhs_iter, limbs_iter_t lhs_end,
                           limb_t rhs) {
  if (lhs_iter == lhs_end) {
    throw underflow_error("Negative number occurred when subtracting");
  } else if (rhs <= *lhs_iter) {
    *lhs_iter -= rhs;
  } else {
    *lhs_iter += LIMB_MODULUS - rhs;

    ++lhs_iter;

    subtract_limb(lhs_iter, lhs_end, 1);
  }
}

void BigInt::subtract_big_int(limbs_iter_t lhs_iter, limbs_iter_t lhs_end,
                              limbs_const_iter_t rhs_iter,
                              limbs_const_iter_t rhs_end) {
  while (rhs_iter != rhs_end) {
    subtract_limb(lhs_iter, lhs_end, *rhs_iter);

    ++lhs_iter;
    ++rhs_iter;
  }
}

BigInt &BigInt::operator-=(limb_t rhs) {
  subtract_limb(limbs.begin(), limbs.end(), rhs);

  return *this;
}

BigInt &BigInt::operator-=(const BigInt &rhs) {
  subtract_big_int(limbs.begin(), limbs.end(), rhs.limbs.cbegin(),
                   rhs.limbs.cend());

  return *this;
}

void BigInt::multiply_by_limb(limbs_t &acc_limbs, limbs_iter_t acc_iter,
                              limbs_const_iter_t lhs_iter,
                              limbs_const_iter_t lhs_end, limb_t rhs) {
  while (lhs_iter != lhs_end) {
    add_limb(acc_limbs, acc_iter, static_cast<double_limb_t>(rhs) * *lhs_iter);

    ++acc_iter;
    ++lhs_iter;
  }
}

void BigInt::multiply_by_big_int(limbs_t &acc_limbs, limbs_iter_t acc_iter,
                                 limbs_const_iter_t lhs_iter,
                                 limbs_const_iter_t lhs_end,
                                 limbs_const_iter_t rhs_iter,
                                 limbs_const_iter_t rhs_end) {
  while (rhs_iter != rhs_end) {
    multiply_by_limb(acc_limbs, acc_iter, lhs_iter, lhs_end, *rhs_iter);

    ++acc_iter;
    ++rhs_iter;
  }
}

BigInt BigInt::operator*(limb_t rhs) const {
  BigInt result;
  multiply_by_limb(result.limbs, result.limbs.begin(), limbs.cbegin(),
                   limbs.cend(), rhs);
  return result;
}

BigInt BigInt::operator*(const BigInt &rhs) const {
  BigInt result;
  multiply_by_big_int(result.limbs, result.limbs.begin(), limbs.cbegin(),
                      limbs.cend(), rhs.limbs.cbegin(), rhs.limbs.cend());
  return result;
}

BigInt::limb_t BigInt::short_division(limbs_iter_t lhs_start,
                                      limbs_iter_t lhs_end,
                                      limbs_const_iter_t rhs_start,
                                      limbs_const_iter_t rhs_end) {
  limb_t result = 0;

  if (rhs_start > first_non_zero(rhs_start, rhs_end)) {
    throw invalid_argument("Divide by zero!");
  }

  while (compare(lhs_start, lhs_end, rhs_start, rhs_end) !=
         Comparison::LESS_THAN) {
    subtract_big_int(lhs_start, lhs_end, rhs_start, rhs_end);
    ++result;
  }

  return result;
}

BigInt::limb_t BigInt::operator/(const BigInt &rhs) {
  return short_division(limbs.begin(), limbs.end(), rhs.limbs.cbegin(),
                        rhs.limbs.cend());
}

ostream &operator<<(ostream &os, const BigInt &value) {
  ios::fmtflags f(os.flags());

  os << std::hex;

  os << "BigInt:";
  for (auto iter = value.limbs.crbegin(); iter != value.limbs.crend(); ++iter) {
    os << *iter;
  }

  os.flags(f);

  return os;
}

void BigInt::trim() { remove_leading_zeros(limbs); }
