#include "bigint.hpp"

BigInt::Limbs::Limbs(const unsigned int quantity) : quantity(quantity) {}

BigInt::BigInt(uint64_t n) {
  while (n > 0) {
    limbs.push_back(n & LIMB_MASK);
    n >>= LIMB_WIDTH;
  }
}

BigInt::BigInt(const string &str) {
  size_t n = str.length() - HEX_CHARS_PER_LIMB;

  while (true) {
    append_limb(str.substr(n, HEX_CHARS_PER_LIMB));

    if (n <= HEX_CHARS_PER_LIMB) {
      break;
    } else {
      n -= HEX_CHARS_PER_LIMB;
    }
  }

  if (n > 0) {
    append_limb(str.substr(0, n));
  }
}

BigInt::limb_t BigInt::least_significant_limb() const {
  return limbs.empty() ? 0 : limbs.front();
}

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

void BigInt::append_limb(string limb_str) {
  if (limb_str.empty()) {
    throw invalid_argument("limb string cannot be empty");
  }

  char *limb_end = nullptr;

  unsigned long int limb = strtoul(limb_str.data(), &limb_end, HEX_MODULUS);

  if (*limb_end != 0) {
    throw invalid_argument("limb string contains invalid character");
  }

  append_limb(limb);
}

void BigInt::append_limb(unsigned long int limb) {
  if (limb > LIMB_MASK) {
    throw invalid_argument("limb value out of range");
  }

  limbs.push_back(limb);
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
                                   limbs_const_iter_t lhs_end, limb_t rhs) {
  limbs_const_iter_t lhs_iter = first_non_zero(lhs_start, lhs_end);

  switch (compare<limbs_const_iter_t>(lhs_iter, lhs_start)) {
    case Comparison::LESS_THAN:
      return compare<limb_t>(0, rhs);
    case Comparison::EQUALS:
      return compare<limb_t>(*lhs_iter, rhs);
    default:
      return Comparison::GREATER_THAN;
  }
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
      Comparison value_comparison = compare<limb_t>(*lhs_iter, *rhs_iter);

      if (value_comparison != Comparison::EQUALS) {
        return value_comparison;
      }

      --lhs_iter;
      --rhs_iter;
    }

    return Comparison::EQUALS;
  }
}

bool operator==(const BigInt &lhs, BigInt::limb_t rhs) {
  return BigInt::compare(lhs.limbs.cbegin(), lhs.limbs.cend(), rhs) ==
         BigInt::Comparison::EQUALS;
}

bool operator<(const BigInt &lhs, const BigInt &rhs) {
  return BigInt::compare(lhs.limbs.cbegin(), lhs.limbs.cend(),
                         rhs.limbs.cbegin(),
                         rhs.limbs.cend()) == BigInt::Comparison::LESS_THAN;
}

bool operator>(const BigInt &lhs, const BigInt &rhs) {
  return BigInt::compare(lhs.limbs.cbegin(), lhs.limbs.cend(),
                         rhs.limbs.cbegin(),
                         rhs.limbs.cend()) == BigInt::Comparison::GREATER_THAN;
}

void BigInt::add_limb(limbs_t &lhs_limbs, limbs_iter_t lhs_iter,
                      double_limb_t rhs) {
  if (rhs > 0) {
    maybe_add_leading_zero(lhs_limbs, lhs_iter);

    double_limb_t carry = 0;

    split_double_limb(rhs + *lhs_iter, carry, *lhs_iter);

    ++lhs_iter;

    if (carry > 0) {
      add_limb(lhs_limbs, lhs_iter, carry);
    }
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

BigInt operator+(const BigInt &lhs, BigInt::limb_t rhs) {
  BigInt result = lhs;
  result += rhs;
  return result;
}

BigInt operator+(const BigInt &lhs, const BigInt &rhs) {
  BigInt result = lhs;
  result += rhs;
  return result;
}

void BigInt::subtract_limb(limbs_iter_t lhs_iter, limbs_iter_t lhs_end,
                           limb_t rhs) {
  if (rhs > 0) {
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

BigInt operator-(const BigInt &lhs, BigInt::limb_t rhs) {
  BigInt result = lhs;
  result -= rhs;
  return result;
}

BigInt operator-(const BigInt &lhs, const BigInt &rhs) {
  BigInt result = lhs;
  result -= rhs;
  return result;
}

void BigInt::multiply_by_limb(limbs_t &acc_limbs, limbs_iter_t acc_iter,
                              limbs_const_iter_t lhs_iter,
                              limbs_const_iter_t lhs_end, limb_t rhs) {
  while (lhs_iter != lhs_end) {
    maybe_add_leading_zero(acc_limbs, acc_iter);
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
    throw domain_error("Divide by zero!");
  }

  while (compare(lhs_start, lhs_end, rhs_start, rhs_end) !=
         Comparison::LESS_THAN) {
    subtract_big_int(lhs_start, lhs_end, rhs_start, rhs_end);
    ++result;
  }

  return result;
}

BigInt BigInt::long_division(limbs_iter_t lhs_start, limbs_iter_t lhs_end,
                             limbs_const_iter_t rhs_start,
                             limbs_const_iter_t rhs_end) {
  BigInt result;

  for (auto lhs_iter = lhs_end; lhs_iter >= lhs_start; --lhs_iter) {
    result.limbs.push_front(
        short_division(lhs_iter, lhs_end, rhs_start, rhs_end));
  }

  return result;
}

void BigInt::div_mod(BigInt &lhs, const BigInt &rhs, BigInt &div) {
  div = long_division(lhs.limbs.begin(), lhs.limbs.end(), rhs.limbs.cbegin(),
                      rhs.limbs.cend());
}

void BigInt::div_mod(const BigInt &lhs, const BigInt &rhs, BigInt &div,
                     BigInt &mod) {
  mod = lhs;
  div_mod(mod, rhs, div);
}

void BigInt::egcd(long a, long b, long &g, long &x, long &y) {
  if (a == 0) {
    g = b;
    x = 0;
    y = 1;
  } else {
    long x_prime, y_prime;
    egcd(b % a, a, g, x_prime, y_prime);

    x = y_prime - (b / a) * x_prime;
    y = x_prime;
  }
}

void BigInt::egcd(const BigInt &a, const BigInt &b, const BigInt &b_orig,
                  BigInt &g, BigInt &x, BigInt &y) {
  if (a == 0) {
    g = b;
    x = BigInt(0);
    y = BigInt(1);
  } else {
    BigInt b_div_a, b_mod_a;
    div_mod(b, a, b_div_a, b_mod_a);

    BigInt x_prime, y_prime;
    egcd(b_mod_a, a, b_orig, g, x_prime, y_prime);

    BigInt y_sub = b_div_a * x_prime;

    while (y_prime < y_sub) {
      y_prime += b_orig;
    }

    x = y_prime - y_sub;
    y = x_prime;
  }
}

long BigInt::mod_inv(long b, long n) {
  long g, x, y;
  egcd(b, n, g, x, y);
  if (g == 1) {
    return x;
  } else {
    throw invalid_argument("cannot calculate mod inv!");
  }
}

BigInt BigInt::mod_inv(const BigInt &b, const BigInt &n) {
  BigInt g, x, y;
  egcd(b, n, n, g, x, y);
  if (g == 1) {
    return x;
  } else {
    throw invalid_argument("cannot calculate mod inv!");
  }
}

BigInt &operator<<=(BigInt &lhs, const BigInt::Limbs &rhs) {
  for (unsigned int n = 0; n < rhs.quantity; ++n) {
    lhs.limbs.push_front(0);
  }

  return lhs;
}

BigInt &operator>>=(BigInt &lhs, const BigInt::Limbs &rhs) {
  for (unsigned int n = 0; n < rhs.quantity; ++n) {
    lhs.limbs.pop_front();
  }

  return lhs;
}

istream &operator>>(istream &is, BigInt &value) {
  string value_str;

  is >> value_str;

  value = BigInt(value_str);

  return is;
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

size_t BigInt::size() const { return limbs.size(); }

void BigInt::trim() { remove_leading_zeros(limbs); }
