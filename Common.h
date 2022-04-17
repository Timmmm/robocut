#pragma once

#include <string>
#include <sstream>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <limits>

#include "Result.h"

// Integer to string.
std::string ItoS(int i);

// Unsigned integer to string.
std::string UItoS(unsigned int i);

// Unsigned long long to string.
std::string ULLtoS(unsigned long long i);

// String to integer.
SResult<int> StoI(std::string_view s);

// String to unsigned integer, returns Fail on fail.
SResult<unsigned int> StoUI(std::string_view s);

// String to unsigned long long, returns Fail on fail.
SResult<unsigned long long> StoULL(std::string_view s);
