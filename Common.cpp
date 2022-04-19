#include "Common.h"

#include <charconv>

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
	std::stringstream s;
	s << i;
	return s.str();
}

template<typename I>
SResult<I> StoInt(std::string_view s)
{
	I value = 0;
	auto result = std::from_chars(s.data(), s.data() + s.size(), value);

	if (result.ec == std::errc::invalid_argument)
		return "Not an integer: " + std::string(s);
	if (result.ec == std::errc::result_out_of_range)
		return "Integer out of range: " + std::string(s);
	if (result.ec != std::errc())
		return "Couldn't parse integer: " + std::string(s);
	if (result.ptr != s.data() + s.size())
		return "Invalid integer characters in string: " + std::string(s);

	return value;
}

SResult<int> StoI(std::string_view s)
{
	return StoInt<int>(s);
}

SResult<unsigned int> StoUI(std::string_view s)
{
	return StoInt<unsigned int>(s);
}

SResult<unsigned long long> StoULL(std::string_view s)
{
	return StoInt<unsigned long long>(s);
}
