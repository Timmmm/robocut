#include "Common.h"

std::string ItoS(int i)
{
	std::stringstream s;
	s << i;
	return s.str();
}

std::string UItoS(unsigned int i)
{
	std::stringstream s;
	s << i;
	return s.str();
}

std::string ULLtoS(unsigned long long i)
{
	char out[128];
	snprintf(out, 128, "%lld", i);
	return out;
}

SResult<int> StoI(std::string_view s)
{
	char* EP;
	const char* P = s.data();
	auto R = strtol(P, &EP, 0);
	if (EP == P)
		return "Couldn't convert string to integer: " + std::string(s);
	if (R < std::numeric_limits<int>::lowest() ||
	        R > std::numeric_limits<int>::max())
		return "Integer out of range: " + std::string(s);
	return static_cast<int>(R);
}

SResult<unsigned int> StoUI(std::string_view s)
{
	char* EP;
	const char* P = s.data();
	auto R = strtoul(P, &EP, 0);
	if (EP == P)
		return "Couldn't convert string to integer: " + std::string(s);
	if (R > std::numeric_limits<unsigned int>::max())
		return "Integer out of range: " + std::string(s);
	return static_cast<unsigned int>(R);
}

SResult<unsigned long long> StoULL(std::string_view s)
{
	char* EP;
	const char* P = s.data();
	auto R = strtoull(P, &EP, 0);
	if (EP == P)
		return "Couldn't convert string to integer: " + std::string(s);
	return R;
}
