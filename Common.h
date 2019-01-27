#pragma once

#include <string>
#include <sstream>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <limits>
#include <variant>

template<typename R>
using Result = std::variant<R, std::string>;

// Integer to string.
std::string ItoS(int i);

// Unsigned integer to string.
std::string UItoS(unsigned int i);

// Unsigned long long to string.
std::string ULLtoS(unsigned long long i);

// String to integer.
Result<int> StoI(std::string_view s);

// String to unsigned integer, returns Fail on fail.
Result<unsigned int> StoUI(std::string_view s);

// String to unsigned long long, returns Fail on fail.
Result<unsigned long long> StoULL(std::string_view s);

// Get an environmental variable.
std::string GetEnv(std::string_view var);
