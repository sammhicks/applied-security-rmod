#pragma once

#include <cstdint>
#include <deque>
#include <iostream>

using std::ios;
using std::deque;
using std::ostream;

class BigInt {
public:
  typedef uint16_t limb_t;
  typedef uint32_t double_limb_t;
  typedef deque<limb_t> limbs_t;
  typedef limbs_t::iterator limbs_iter_t;
  typedef limbs_t::const_iterator limbs_const_iter_t;

private:
  limbs_t limbs;

  // Add a leading zero if the iterator is off the end
  static void maybe_add_leading_zero(limbs_t &limbs, limbs_iter_t &iter);

  // Split a double limb into two limbs
  static void split_double_limb(double_limb_t d, double_limb_t &large,
                                limb_t &small);

  // lhs += rhs
  static void add_limb(limbs_t &lhs_limbs, limbs_iter_t lhs_iter,
                       double_limb_t rhs);
  static void add_big_int(limbs_t &lhs_limbs, limbs_iter_t lhs_iter,
                          const limbs_t &rhs_limbs,
                          limbs_const_iter_t rhs_iter);

  // acc += lhs * rhs
  static void multiply_by_limb(limbs_t &acc_limbs, limbs_iter_t acc_iter,
                               const limbs_t &lhs_limbs,
                               limbs_const_iter_t lhs_iter, limb_t rhs);
  static void multiply_by_big_int(limbs_t &acc_limbs, limbs_iter_t acc_iter,
                                  const limbs_t &lhs_limbs,
                                  limbs_const_iter_t lhs_iter,
                                  const limbs_t &rhs_limbs,
                                  limbs_const_iter_t rhs_iter);

public:
  static const double_limb_t LIMB_WIDTH;
  static const double_limb_t LIMB_MASK;

  BigInt &operator+=(limb_t rhs);
  BigInt &operator+=(const BigInt &rhs);

  BigInt operator*(limb_t rhs);
  BigInt operator*(const BigInt &rhs);

  friend ostream &operator<<(ostream &os, const BigInt &value);
};
