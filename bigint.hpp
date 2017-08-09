#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <stdexcept>
#include <string>

//#include "list.hpp"

using std::ios;
using std::domain_error;
using std::invalid_argument;
using std::istream;
using std::ostream;
using std::string;
using std::underflow_error;
using std::range_error;

class BigInt {
public:
  class Limbs {
  public:
    const unsigned int quantity;

    Limbs(const unsigned int quantity);
  };

  typedef uint8_t bit_count_type;
  typedef uint16_t limb_type;
  typedef uint32_t double_limb_type;
  typedef std::deque<limb_type> limbs_type;
  typedef limbs_type::size_type limbs_index_type;
  typedef limbs_type::const_iterator limbs_const_iter_type;
  typedef limbs_type::const_reverse_iterator limbs_const_reverse_iter_type;
  typedef limbs_type::difference_type limbs_index_difference_type;
  typedef limbs_type::size_type limbs_size_type;
  typedef limbs_type::difference_type bit_index_type;

  static constexpr bit_count_type HEX_BITS = 4;
  static constexpr size_t HEX_MODULUS = 1 << BigInt::HEX_BITS;
  static constexpr bit_count_type LIMB_WIDTH = 16;
  static constexpr double_limb_type LIMB_MODULUS = 1 << BigInt::LIMB_WIDTH;
  static constexpr double_limb_type LIMB_MASK = BigInt::LIMB_MODULUS - 1;
  static constexpr bit_count_type HEX_CHARS_PER_LIMB = LIMB_WIDTH / HEX_BITS;

private:
  limbs_type limbs;

  // Add a leading zero if the iterator is off the end
  static void maybe_add_leading_zero(limbs_type &limbs, limbs_index_type index);

  // Remove leading zeros
  static void remove_leading_zeros(limbs_type &limbs);

  // Add a limb as the most significant limb
  void append_limb(string limb_str);
  void append_limb(unsigned long int limb);

  // Split a double limb into two limbs
  static void split_double_limb(double_limb_type d, double_limb_type &large,
                                limb_type &small);

  // Move lhs_iter backwards until it reaches lhs_start of a non-zero limb
  static limbs_const_iter_type first_non_zero(limbs_const_iter_type start,
                                              limbs_const_iter_type end);

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

  static Comparison compare(limbs_const_iter_type lhs_start,
                            limbs_const_iter_type lhs_end, limb_type rhs);

  static Comparison compare(limbs_const_iter_type lhs_start,
                            limbs_const_iter_type lhs_end,
                            limbs_const_iter_type rhs_start,
                            limbs_const_iter_type rhs_end);

  // lhs += rhs
  static void add_limb(limbs_type &lhs_limbs, limbs_index_type lhs_index,
                       double_limb_type rhs);
  static void add_big_int(limbs_type &lhs_limbs, limbs_index_type lhs_index,
                          limbs_const_iter_type rhs_iter,
                          limbs_const_iter_type rhs_end);

  // lhs -= rhs
  static void subtract_limb(limbs_type &lhs_limbs, limbs_index_type lhs_index,
                            limb_type rhs);
  static void subtract_big_int(limbs_type &lhs_limbs,
                               limbs_index_type lhs_index,
                               limbs_const_iter_type rhs_iter,
                               limbs_const_iter_type rhs_end);

  // acc += lhs * rhs
  static void multiply_by_limb(limbs_type &acc_limbs,
                               limbs_index_type acc_index,
                               limbs_const_iter_type lhs_iter,
                               limbs_const_iter_type lhs_end, limb_type rhs);
  static void multiply_by_big_int(limbs_type &acc_limbs,
                                  limbs_index_type acc_index,
                                  limbs_const_iter_type lhs_iter,
                                  limbs_const_iter_type lhs_end,
                                  limbs_const_iter_type rhs_iter,
                                  limbs_const_iter_type rhs_end);

  static limb_type short_division(limbs_type &lhs_limbs,
                                  limbs_index_type lhs_index,
                                  limbs_const_iter_type rhs_start,
                                  limbs_const_iter_type rhs_end);

  static BigInt long_division(limbs_type &lhs_limbs, limbs_index_type lhs_index,
                              limbs_const_iter_type rhs_start,
                              limbs_const_iter_type rhs_end);

  static void egcd(long a, long b, long &g, long &x, long &y);
  static void egcd(const BigInt &a, const BigInt &b, const BigInt &b_orig,
                   BigInt &g, BigInt &x, BigInt &y);

public:
  BigInt() = default;
  BigInt(uint64_t n);
  BigInt(const string &str);

  limb_type least_significant_limb() const;

  friend bool operator==(const BigInt &lhs, limb_type rhs);

  friend bool operator<(const BigInt &lhs, const BigInt &rhs);
  friend bool operator>(const BigInt &lhs, const BigInt &rhs);

  BigInt &operator+=(limb_type rhs);
  BigInt &operator+=(const BigInt &rhs);

  BigInt &operator-=(limb_type rhs);
  BigInt &operator-=(const BigInt &rhs);

  BigInt operator*(limb_type rhs) const;
  BigInt operator*(const BigInt &rhs) const;

  static void div_mod(BigInt &lhs, const BigInt &rhs, BigInt &div);
  static void div_mod(const BigInt &lhs, const BigInt &rhs, BigInt &div,
                      BigInt &mod);

  BigInt &operator%=(const BigInt &rhs);

  static long mod_inv(long b, long n);
  static BigInt mod_inv(const BigInt &b, const BigInt &n);

  bit_index_type log_2() const;
  int operator[](const bit_index_type index) const;

  friend BigInt &operator<<=(BigInt &lhs, const Limbs &rhs);
  friend BigInt &operator>>=(BigInt &lhs, const Limbs &rhs);

  friend istream &operator>>(istream &is, BigInt &value);
  friend ostream &operator<<(ostream &os, const BigInt &value);

  // The number of limbs
  limbs_size_type limb_count() const;

  // Remove leading zeros
  void trim();
};

BigInt operator+(const BigInt &lhs, BigInt::limb_type rhs);
BigInt operator+(const BigInt &lhs, const BigInt &rhs);

BigInt operator-(const BigInt &lhs, BigInt::limb_type rhs);
BigInt operator-(const BigInt &lhs, const BigInt &rhs);

bool operator!=(const BigInt &lhs, const BigInt &rhs);
bool operator<=(const BigInt &lhs, const BigInt &rhs);
bool operator>=(const BigInt &lhs, const BigInt &rhs);
