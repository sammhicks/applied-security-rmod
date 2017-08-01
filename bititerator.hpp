#pragma once

#include "bigint.hpp"

class BitIterator {
private:
  BigInt::limbs_const_reverse_iter_t current_limb;
  BigInt::bit_count_t bits_remaining;

  ptrdiff_t bit_acc;

public:
  BitIterator(BigInt::limbs_const_reverse_iter_t current_limb,
              BigInt::bit_count_t bits_remaining);

  ptrdiff_t get_acc() const;
  void clear_acc();

  bool operator*() const;
  void operator++();
  bool operator!=(const BitIterator &other) const;
};
