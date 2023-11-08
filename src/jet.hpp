#pragma once

#include <vector>
#include <string>

namespace jet
{
	std::vector<std::string> split(const std::string& str, const std::string& delim = " ");
	std::vector<std::string> split(const std::string& str, const std::string& delim);
}