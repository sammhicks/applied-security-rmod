#pragma once

#include <climits>
#include <cstdint>
#include <deque>
#include <iostream>
#include <stdexcept>
#include <string>

using std::ios;
using std::deque;
using std::domain_error;
using std::invalid_argument;
using std::istream;
using std::ostream;
using std::string;
using std::underflow_error;

class BigInt {
public:
  class Limbs {
  public:
    const unsigned int quantity;

    Limbs(const unsigned int quantity);
  };

  typedef uint16_t limb_t;
  typedef uint32_t double_limb_t;
  typedef deque<limb_t> limbs_t;
  typedef limbs_t::iterator limbs_iter_t;
  typedef limbs_t::const_iterator limbs_const_iter_t;
  typedef limbs_t::difference_type limbs_difference_t;

  static constexpr size_t HEX_BITS = 4;
  static constexpr size_t HEX_MODULUS = 1 << BigInt::HEX_BITS;
  static constexpr size_t LIMB_WIDTH = 4;
  static constexpr double_limb_t LIMB_MODULUS = 1 << BigInt::LIMB_WIDTH;
  static constexpr double_limb_t LIMB_MASK = BigInt::LIMB_MODULUS - 1;
  static constexpr size_t HEX_CHARS_PER_LIMB = LIMB_WIDTH / HEX_BITS;

private:
  limbs_t limbs;

  // Add a leading zero if the iterator is off the end
  static void maybe_add_leading_zero(limbs_t &limbs, limbs_iter_t &iter);

  // Remove leading zeros
  static void remove_leading_zeros(limbs_t &limbs);

  // Add a limb as the most significant limb
  void append_limb(string limb_str);
  void append_limb(unsigned long int limb);

  // Split a double limb into two limbs
  static void split_double_limb(double_limb_t d, double_limb_t &large,
                                limb_t &small);

  // Move lhs_iter backwards until it reaches lhs_start of a non-zero limb
  static limbs_const_iter_t first_non_zero(limbs_const_iter_t start,
                                           limbs_const_iter_t end);

  enum class Comparison { LESS_THAN = -1, EQUALS = 0, GREATER_THAN = 1 };

  template <typename T> static Comparison compare(T a, T b) {
    if (a == b) {
      return Comparison::EQUALS;
    } else if (a < b) {
      return Comparison::LESS_THAN;
    } else {
      return Comparison::GREATER_THAN;
    }
  }

  static Comparison compare(limbs_const_iter_t lhs_start,
                            limbs_const_iter_t lhs_end, limb_t rhs);

  static Comparison compare(limbs_const_iter_t lhs_start,
                            limbs_const_iter_t lhs_end,
                            limbs_const_iter_t rhs_start,
                            limbs_const_iter_t rhs_end);

  // lhs += rhs
  static void add_limb(limbs_t &lhs_limbs, limbs_iter_t lhs_iter,
                       double_limb_t rhs);
  static void add_big_int(limbs_t &lhs_limbs, limbs_iter_t lhs_iter,
                          limbs_const_iter_t rhs_iter,
                          limbs_const_iter_t rhs_end);

  // lhs -= rhs
  static void subtract_limb(limbs_iter_t lhs_iter, limbs_iter_t lhs_end,
                            limb_t rhs);
  static void subtract_big_int(limbs_iter_t lhs_iter, limbs_iter_t lhs_end,
                               limbs_const_iter_t rhs_iter,
                               limbs_const_iter_t rhs_end);

  // acc += lhs * rhs
  static void multiply_by_limb(limbs_t &acc_limbs, limbs_iter_t acc_iter,
                               limbs_const_iter_t lhs_iter,
                               limbs_const_iter_t lhs_end, limb_t rhs);
  static void multiply_by_big_int(limbs_t &acc_limbs, limbs_iter_t acc_iter,
                                  limbs_const_iter_t lhs_iter,
                                  limbs_const_iter_t lhs_end,
                                  limbs_const_iter_t rhs_iter,
                                  limbs_const_iter_t rhs_end);

  static limb_t short_division(limbs_iter_t lhs_start, limbs_iter_t lhs_end,
                               limbs_const_iter_t rhs_start,
                               limbs_const_iter_t rhs_end);

  static BigInt long_division(limbs_iter_t lhs_start, limbs_iter_t lhs_end,
                              limbs_const_iter_t rhs_start,
                              limbs_const_iter_t rhs_end);

  static void egcd(long a, long b, long &g, long &x, long &y);
  static void egcd(const BigInt &a, const BigInt &b, const BigInt &b_orig,
                   BigInt &g, BigInt &x, BigInt &y);

public:
  BigInt() = default;
  BigInt(uint64_t n);
  BigInt(const string &str);

  limb_t least_significant_limb() const;

  friend bool operator==(const BigInt &lhs, limb_t rhs);

  friend bool operator<(const BigInt &lhs, const BigInt &rhs);
  friend bool operator>(const BigInt &lhs, const BigInt &rhs);

  BigInt &operator+=(limb_t rhs);
  BigInt &operator+=(const BigInt &rhs);

  BigInt &operator-=(limb_t rhs);
  BigInt &operator-=(const BigInt &rhs);

  BigInt operator*(limb_t rhs) const;
  BigInt operator*(const BigInt &rhs) const;

  static void div_mod(BigInt &lhs, const BigInt &rhs, BigInt &div);
  static void div_mod(const BigInt &lhs, const BigInt &rhs, BigInt &div,
                      BigInt &mod);

  static long mod_inv(long b, long n);
  static BigInt mod_inv(const BigInt &b, const BigInt &n);

  friend BigInt &operator<<=(BigInt &lhs, const Limbs &rhs);
  friend BigInt &operator>>=(BigInt &lhs, const Limbs &rhs);

  friend istream &operator>>(istream &is, BigInt &value);
  friend ostream &operator<<(ostream &os, const BigInt &value);

  // The number of limbs
  size_t size();

  // Remove leading zeros
  void trim();
};

BigInt operator+(const BigInt &lhs, BigInt::limb_t rhs);
BigInt operator+(const BigInt &lhs, const BigInt &rhs);

BigInt operator-(const BigInt &lhs, BigInt::limb_t rhs);
BigInt operator-(const BigInt &lhs, const BigInt &rhs);
