#include "ft.hpp"
#include <sstream>

std::string ft::toString(int const nb)
{
	std::stringstream ss;

	ss << nb;
	return ss.str();
}
