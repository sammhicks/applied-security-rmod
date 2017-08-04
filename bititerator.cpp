#include "bititerator.hpp"

BitIterator::BitIterator(const BigInt &value)
    : current_limb(value.limbs.crbegin()), end_limb(value.limbs.crend()),
      bits_remaining(BigInt::LIMB_WIDTH), bit_acc(0) {
  while (!**this) {
    ++*this;
  }
}

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

void BitIterator::operator--() {
  bit_acc >>= 1;
  // Clear the LSB
  bit_acc &= (~1);

  ++bits_remaining;

  if (bits_remaining > BigInt::LIMB_WIDTH) {
    --current_limb;
    bits_remaining = 1;
  }
}

bool BitIterator::operator==(const BitIterator &other) const {
  return (current_limb == other.current_limb) &&
         (bits_remaining == other.bits_remaining);
}

bool BitIterator::operator!=(const BitIterator &other) const {
  return !(*this == other);
}

bool BitIterator::is_finished() const { return current_limb == end_limb; }
