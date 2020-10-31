#pragma once

#include <string>
#include <vector>

class machine {
	std::string name;
};

class site {
	std::string name;
	std::vector<machine> machines;
};