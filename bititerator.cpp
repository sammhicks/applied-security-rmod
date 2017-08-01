#include "bititerator.hpp"

BitIterator::BitIterator(BigInt::limbs_const_reverse_iter_t current_limb,
                         BigInt::bit_count_t bits_remaining)
    : current_limb(current_limb), bits_remaining(bits_remaining), bit_acc(0) {}

ptrdiff_t BitIterator::get_acc() const { return bit_acc + (**this ? 1 : 0); }

void BitIterator::clear_acc() { bit_acc = 0; }

bool BitIterator::operator*() const {
  return ((*current_limb >> (bits_remaining - 1)) & 1) > 0;
}

void BitIterator::operator++() {
  bit_acc += **this ? 1 : 0;
  bit_acc <<= 1;

  --bits_remaining;

  if (bits_remaining == 0) {
    ++current_limb;
    bits_remaining = BigInt::LIMB_WIDTH;
  }
}

bool BitIterator::operator!=(const BitIterator &other) const {
  return (current_limb == other.current_limb) &&
         (bits_remaining == other.bits_remaining);
}
