#pragma once

#include <string>
#include <vector>

struct channel {
	channel(const int id, std::string name, std::string units) : name(std::move(name)), units(std::move(units)), id(id) {}

	int id;
	std::string name;
	std::string units;
};

struct machine {
	machine(int id, std::string name) : id(id), name(std::move(name)) {}

	int id;
	std::string name;
	std::vector<channel> channels;
};

struct site {
	site(const int id, std::string name) : id(id), name(std::move(name)) {}

	int id;
	std::string name;
	std::vector<machine> machines;
};