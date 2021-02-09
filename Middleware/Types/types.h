#pragma once

#include <string>
#include <vector>
#include <chrono>

using namespace std::chrono;
using time_point_t = system_clock::time_point;

struct channel {
	explicit channel(const int id) : id(id) {}
	channel(const int id, std::string name, std::string units) : name(std::move(name)), units(std::move(units)), id(id) {}

	int id;
	std::string name;
	std::string units;
};

struct machine {
	machine(int id) : id(id) {}

	machine(int id, std::string name) : id(id), name(std::move(name)) {}

	int id;
	std::string name;
	std::vector<channel> channels;
};

struct site {
	site(const int id) : id(id) {}

	site(const int id, std::string name) : id(id), name(std::move(name)) {}

	int id;
	std::string name;
	std::vector<machine> machines;
};