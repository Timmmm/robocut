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
inline std::string ItoS(int I)
{
	std::stringstream S;
	S << I;
	return S.str();
}

// Unsigned integer to string.
inline std::string UItoS(unsigned int I)
{
	std::stringstream S;
	S << I;
	return S.str();
}

// Unsigned long long to string.
inline std::string ULLtoS(unsigned long long I)
{
	char out[128];
	sprintf(out, "%lld", I);
	return out;
}

// String to integer.
inline Result<int> StoI(const std::string& S)
{
	char* EP;
	const char* P = S.c_str();
	auto R = strtol(P, &EP, 0);
	if (EP == P)
		return "Couldn't convert string to integer: " + S;
	if (R < std::numeric_limits<int>::lowest() ||
	    R > std::numeric_limits<int>::max())
		return "Integer out of range: " + S;
	return static_cast<int>(R);
}

// String to unsigned integer, returns Fail on fail.
inline Result<unsigned int> StoUI(const std::string& S)
{
	char* EP;
	const char* P = S.c_str();
	auto R = strtoul(P, &EP, 0);
	if (EP == P)
		return "Couldn't convert string to integer: " + S;
	if (R > std::numeric_limits<unsigned int>::max())
		return "Integer out of range: " + S;
	return static_cast<unsigned int>(R);
}

// String to unsigned long long, returns Fail on fail.
inline Result<unsigned long long> StoULL(const std::string& S)
{
	char* EP;
	const char* P = S.c_str();
	auto R = strtoull(P, &EP, 0);
	if (EP == P)
		return "Couldn't convert string to integer: " + S;
	return R;
}

// Get an environmental variable.
inline std::string GetEnv(const std::string& Var)
{
	char* V = getenv(Var.c_str());
	if (!V)
		return "";
	return V;
}
