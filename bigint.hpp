#pragma once

#include <cstdint>
#include <deque>
#include <iostream>
#include <stdexcept>

using std::ios;
using std::deque;
using std::invalid_argument;
using std::ostream;
using std::underflow_error;

class BigInt {
public:
  typedef uint16_t limb_t;
  typedef uint32_t double_limb_t;
  typedef deque<limb_t> limbs_t;
  typedef limbs_t::iterator limbs_iter_t;
  typedef limbs_t::const_iterator limbs_const_iter_t;
  typedef limbs_t::difference_type limbs_difference_t;

private:
  limbs_t limbs;

  // Add a leading zero if the iterator is off the end
  static void maybe_add_leading_zero(limbs_t &limbs, limbs_iter_t &iter);

  // Remove leading zeros
  static void remove_leading_zeros(limbs_t &limbs);

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

public:
  static const double_limb_t LIMB_WIDTH;
  static const double_limb_t LIMB_MODULUS;
  static const double_limb_t LIMB_MASK;

  BigInt &operator+=(limb_t rhs);
  BigInt &operator+=(const BigInt &rhs);

  BigInt &operator-=(limb_t rhs);
  BigInt &operator-=(const BigInt &rhs);

  BigInt operator*(limb_t rhs) const;
  BigInt operator*(const BigInt &rhs) const;

  limb_t operator/(const BigInt &rhs);

  friend ostream &operator<<(ostream &os, const BigInt &value);

  void trim();
};
