#pragma once

#include <cstdlib>
#include <iostream>

#include "bigint.hpp"
#include "modint.hpp"

using std::cin;
using std::cout;
using std::endl;

void repeat_stage(void (*stage)());

void stage1();
void stage2();
void stage3();

int main(int argc, char *argv[]);
