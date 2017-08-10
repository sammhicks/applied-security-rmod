#pragma once

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "bigint.hpp"

using std::ifstream;

#define FIX_KEY

void seed_generator();

BigInt::limb_type random_limb();

BigInt::limb_type random_limb(BigInt::limb_type range);

BigInt random_bigint(const BigInt &range);

BigInt random_bigint(const BigInt &lower, const BigInt &upper);
