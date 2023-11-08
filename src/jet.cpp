#include <string>
#include <vector>
#include "jet.hpp"

namespace jet
{
	std::vector<std::string> split(const std::string& str, const std::string& delim)
	{
		std::vector<std::string> res;
		std::size_t current, previous = 0;
		current = str.find(delim);
		while (current != std::string::npos) {
			res.push_back(str.substr(previous, current - previous));
			previous = current + delim.length();
			current = str.find(delim, previous);
		}
		res.push_back(str.substr(previous, current - previous));
		return res;
	}
}