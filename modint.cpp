#include "modint.hpp"

ModInt::ModInt(BigInt value, const ModIntFactory &factory)
    : value(value), factory(factory) {}
