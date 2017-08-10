#include "bigint.hpp"

BigInt::Limbs::Limbs(const unsigned int quantity) : quantity(quantity) {}

BigInt::BigInt(uint64_t n) {
  while (n > 0) {
    limbs.push_back(n & LIMB_MASK);
    n >>= LIMB_WIDTH;
  }
}

BigInt::BigInt(const string &str) {
  if (!str.empty()) {
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
}

BigInt::limbs_const_iter_type BigInt::most_significant_limb() const {
  return first_non_zero(limbs.cbegin(), limbs.cend());
}

BigInt::limbs_const_iter_type BigInt::least_significant_limb() const {
  return limbs.cbegin();
}

BigInt::limb_type BigInt::least_significant_limb_value() const {
  return limbs.empty() ? 0 : limbs.front();
}

void BigInt::maybe_add_leading_zero(limbs_type &limbs, limbs_index_type index) {
  while (index >= limbs.size()) {
    limbs.push_back(0);
  }
}

void BigInt::remove_leading_zeros(limbs_type &limbs) {
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

void BigInt::split_double_limb(double_limb_type d, double_limb_type &large,
                               limb_type &small) {
  small = d & LIMB_MASK;
  large = (d >> LIMB_WIDTH) & LIMB_MASK;
}

BigInt::limbs_const_iter_type
BigInt::first_non_zero(limbs_const_iter_type start, limbs_const_iter_type end) {
  limbs_const_iter_type iter = end;
  --iter;
  while (iter >= start && *iter == 0) {
    --iter;
  }

  return iter;
}

BigInt::Comparison BigInt::compare(limbs_const_iter_type lhs_start,
                                   limbs_const_iter_type lhs_end,
                                   limb_type rhs) {
  limbs_const_iter_type lhs_iter = first_non_zero(lhs_start, lhs_end);

  switch (compare<limbs_const_iter_type>(lhs_iter, lhs_start)) {
    case Comparison::LESS_THAN:
      return compare<limb_type>(0, rhs);
    case Comparison::EQUALS:
      return compare<limb_type>(*lhs_iter, rhs);
    default:
      return Comparison::GREATER_THAN;
  }
}

BigInt::Comparison BigInt::compare(limbs_const_iter_type lhs_start,
                                   limbs_const_iter_type lhs_end,
                                   limbs_const_iter_type rhs_start,
                                   limbs_const_iter_type rhs_end) {
  limbs_const_iter_type lhs_iter = first_non_zero(lhs_start, lhs_end);
  limbs_const_iter_type rhs_iter = first_non_zero(rhs_start, rhs_end);

  limbs_const_iter_type::difference_type lhs_length = lhs_iter - lhs_start + 1;
  limbs_const_iter_type::difference_type rhs_length = rhs_iter - rhs_start + 1;

  Comparison length_comparison =
      compare<limbs_index_difference_type>(lhs_length, rhs_length);

  if (length_comparison != Comparison::EQUALS) {
    return length_comparison;
  } else {
    while (lhs_iter >= lhs_start && rhs_iter >= rhs_start) {
      Comparison value_comparison = compare<limb_type>(*lhs_iter, *rhs_iter);

      if (value_comparison != Comparison::EQUALS) {
        return value_comparison;
      }

      --lhs_iter;
      --rhs_iter;
    }

    return Comparison::EQUALS;
  }
}

bool operator==(const BigInt &lhs, BigInt::limb_type rhs) {
  return BigInt::compare(lhs.limbs.cbegin(), lhs.limbs.cend(), rhs) ==
         BigInt::Comparison::EQUALS;
}

bool operator==(const BigInt &lhs, const BigInt &rhs) {
  return BigInt::compare(lhs.limbs.cbegin(), lhs.limbs.cend(),
                         rhs.limbs.cbegin(),
                         rhs.limbs.cend()) == BigInt::Comparison::EQUALS;
}

bool operator!=(const BigInt &lhs, BigInt::limb_type rhs) {
  return !(lhs == rhs);
}

bool operator!=(const BigInt &lhs, const BigInt &rhs) { return !(lhs == rhs); }

bool operator<(const BigInt &lhs, BigInt::limb_type rhs) {
  return BigInt::compare(lhs.limbs.cbegin(), lhs.limbs.cend(), rhs) ==
         BigInt::Comparison::LESS_THAN;
}

bool operator<(const BigInt &lhs, const BigInt &rhs) {
  return BigInt::compare(lhs.limbs.cbegin(), lhs.limbs.cend(),
                         rhs.limbs.cbegin(),
                         rhs.limbs.cend()) == BigInt::Comparison::LESS_THAN;
}

bool operator<=(const BigInt &lhs, BigInt::limb_type rhs) {
  return !(lhs > rhs);
}

bool operator<=(const BigInt &lhs, const BigInt &rhs) { return !(lhs > rhs); }

bool operator>(const BigInt &lhs, BigInt::limb_type rhs) {
  return BigInt::compare(lhs.limbs.cbegin(), lhs.limbs.cend(), rhs) ==
         BigInt::Comparison::GREATER_THAN;
}

bool operator>(const BigInt &lhs, const BigInt &rhs) {
  return BigInt::compare(lhs.limbs.cbegin(), lhs.limbs.cend(),
                         rhs.limbs.cbegin(),
                         rhs.limbs.cend()) == BigInt::Comparison::GREATER_THAN;
}

bool operator>=(const BigInt &lhs, BigInt::limb_type rhs) {
  return !(lhs < rhs);
}

bool operator>=(const BigInt &lhs, const BigInt &rhs) { return !(lhs < rhs); }

void BigInt::add_limb(limbs_type &lhs_limbs, limbs_index_type lhs_index,
                      double_limb_type rhs) {
  if (rhs > 0) {
    maybe_add_leading_zero(lhs_limbs, lhs_index);
    double_limb_type carry = 0;

    split_double_limb(rhs + lhs_limbs[lhs_index], carry, lhs_limbs[lhs_index]);

    if (carry > 0) {
      add_limb(lhs_limbs, lhs_index + 1, carry);
    }
  }
}

void BigInt::add_big_int(limbs_type &lhs_limbs, limbs_index_type lhs_index,
                         limbs_const_iter_type rhs_iter,
                         limbs_const_iter_type rhs_end) {
  while (rhs_iter < rhs_end) {
    add_limb(lhs_limbs, lhs_index, *rhs_iter);

    ++lhs_index;
    ++rhs_iter;
  }
}

BigInt &BigInt::operator+=(limb_type rhs) {
  add_limb(limbs, 0, rhs);

  return *this;
}

BigInt &BigInt::operator+=(const BigInt &rhs) {
  add_big_int(limbs, 0, rhs.limbs.cbegin(), rhs.limbs.cend());

  return *this;
}

BigInt operator+(const BigInt &lhs, BigInt::limb_type rhs) {
  BigInt result = lhs;
  result += rhs;
  return result;
}

BigInt operator+(const BigInt &lhs, const BigInt &rhs) {
  BigInt result = lhs;
  result += rhs;
  return result;
}

void BigInt::subtract_limb(limbs_type &lhs_limbs, limbs_index_type lhs_index,
                           limb_type rhs) {
  if (rhs > 0) {
    if (lhs_index == lhs_limbs.size()) {
      throw underflow_error("Negative number occurred when subtracting");
    } else if (rhs <= lhs_limbs[lhs_index]) {
      lhs_limbs[lhs_index] -= rhs;
    } else {
      lhs_limbs[lhs_index] += LIMB_MODULUS - rhs;

      subtract_limb(lhs_limbs, lhs_index + 1, 1);
    }
  }
}

void BigInt::subtract_big_int(limbs_type &lhs_limbs, limbs_index_type lhs_index,
                              limbs_const_iter_type rhs_iter,
                              limbs_const_iter_type rhs_end) {
  while (rhs_iter != rhs_end) {
    subtract_limb(lhs_limbs, lhs_index, *rhs_iter);

    ++lhs_index;
    ++rhs_iter;
  }
}

BigInt &BigInt::operator-=(limb_type rhs) {
  subtract_limb(limbs, 0, rhs);

  return *this;
}

BigInt &BigInt::operator-=(const BigInt &rhs) {
  subtract_big_int(limbs, 0, rhs.limbs.cbegin(), rhs.limbs.cend());

  return *this;
}

BigInt operator-(const BigInt &lhs, BigInt::limb_type rhs) {
  BigInt result = lhs;
  result -= rhs;
  return result;
}

BigInt operator-(const BigInt &lhs, const BigInt &rhs) {
  BigInt result = lhs;
  result -= rhs;
  return result;
}

void BigInt::multiply_by_limb(limbs_type &acc_limbs, limbs_index_type acc_index,
                              limbs_const_iter_type lhs_iter,
                              limbs_const_iter_type lhs_end, limb_type rhs) {
  if (lhs_iter != lhs_end) {
    add_limb(acc_limbs, acc_index,
             static_cast<double_limb_type>(rhs) * *lhs_iter);

    multiply_by_limb(acc_limbs, acc_index + 1, lhs_iter + 1, lhs_end, rhs);
  }
}

void BigInt::multiply_by_big_int(limbs_type &acc_limbs,
                                 limbs_index_type acc_index,
                                 limbs_const_iter_type lhs_iter,
                                 limbs_const_iter_type lhs_end,
                                 limbs_const_iter_type rhs_iter,
                                 limbs_const_iter_type rhs_end) {
  while (rhs_iter != rhs_end) {
    multiply_by_limb(acc_limbs, acc_index, lhs_iter, lhs_end, *rhs_iter);

    ++acc_index;
    ++rhs_iter;
  }
}

BigInt BigInt::operator*(limb_type rhs) const {
  BigInt result;
  multiply_by_limb(result.limbs, 0, limbs.cbegin(), limbs.cend(), rhs);
  return result;
}

BigInt BigInt::operator*(const BigInt &rhs) const {
  BigInt result;

  multiply_by_big_int(result.limbs, 0, limbs.cbegin(), limbs.cend(),
                      rhs.limbs.cbegin(), rhs.limbs.cend());
  return result;
}

BigInt::limb_type BigInt::short_division(limbs_type &lhs_limbs,
                                         limbs_index_type lhs_index,
                                         limbs_const_iter_type rhs_start,
                                         limbs_const_iter_type rhs_end) {
  limb_type result = 0;

  if (rhs_start > first_non_zero(rhs_start, rhs_end)) {
    throw domain_error("Divide by zero!");
  }

  while (compare(lhs_limbs.cbegin() + lhs_index, lhs_limbs.cend(), rhs_start,
                 rhs_end) != Comparison::LESS_THAN) {
    subtract_big_int(lhs_limbs, lhs_index, rhs_start, rhs_end);
    ++result;
  }

  return result;
}

BigInt BigInt::long_division(limbs_type &lhs_limbs, limbs_index_type lhs_index,
                             limbs_const_iter_type rhs_start,
                             limbs_const_iter_type rhs_end) {
  BigInt result;

  while (true) {
    result.limbs.push_front(
        short_division(lhs_limbs, lhs_index, rhs_start, rhs_end));

    if (lhs_index == 0) {
      break;
    } else {
      --lhs_index;
    }
  }

  return result;
}

void BigInt::div_mod(BigInt &lhs, const BigInt &rhs, BigInt &div) {
  div = long_division(lhs.limbs, lhs.limbs.size() - 1, rhs.limbs.cbegin(),
                      rhs.limbs.cend());
}

void BigInt::div_mod(const BigInt &lhs, const BigInt &rhs, BigInt &div,
                     BigInt &mod) {
  mod = lhs;
  div_mod(mod, rhs, div);
}

BigInt &BigInt::operator%=(const BigInt &rhs) {
  long_division(limbs, limbs.size() - 1, rhs.limbs.cbegin(), rhs.limbs.cend());
  return *this;
}

// https://en.wikibooks.org/wiki/Algorithm_Implementation/Mathematics/Extended_Euclidean_algorithm#Recursive_algorithm_2
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

// https://en.wikibooks.org/wiki/Algorithm_Implementation/Mathematics/Extended_Euclidean_algorithm#Recursive_algorithm_2
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

BigInt::bit_index_type BigInt::log_2() const {
  return limb_count() * LIMB_WIDTH;
}

int BigInt::operator[](const BigInt::bit_index_type index) const {
  if (index < 0) {
    throw range_error("Cannot access negative bit");
  } else {
    limbs_index_type limb_index = index / LIMB_WIDTH;
    bit_index_type bit_index = index % LIMB_WIDTH;

    if (limb_index >= limb_count()) {
      return 0;
    } else {
      return (limbs[limb_index] >> bit_index) & 1;
    }
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

  if (!value.limbs.empty()) {
    ios::fmtflags f(os.flags());

    os << std::hex;
    os << std::uppercase;

    os << value.limbs.back();

    for (auto iter = value.limbs.crbegin() + 1; iter != value.limbs.crend();
         ++iter) {
      os << std::setfill('0') << std::setw(BigInt::HEX_CHARS_PER_LIMB) << *iter;
    }

    os.flags(f);
  } else {
    os << "0";
  }

  return os;
}

BigInt::limbs_size_type BigInt::limb_count() const { return limbs.size(); }

void BigInt::trim() { remove_leading_zeros(limbs); }
