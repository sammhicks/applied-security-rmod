#include "modintfactory.hpp"

ModIntFactory::ModIntFactory(BigInt mod) : mod(mod) {
  mod.trim();
  range += 1;

  range <<= BigInt::Limbs(mod.size());
}
