#pragma once

#include "bigint.hpp"

class BitIterator {
private:
  BigInt::limbs_const_reverse_iter_t current_limb;
  const BigInt::limbs_const_reverse_iter_t end_limb;
  BigInt::bit_count_t bits_remaining;

  ptrdiff_t bit_acc;

public:
  BitIterator(const BigInt &value);

  ptrdiff_t get_acc() const;
  size_t get_acc_count() const;
  void clear_acc();

  bool operator*() const;
  void operator++();
  void operator--();
  bool operator==(const BitIterator &other) const;
  bool operator!=(const BitIterator &other) const;

  bool is_finished() const;
};
