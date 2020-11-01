#pragma once

#include "IDatabase.h"

struct TestDatabase : public IDatabase {

	std::vector<site> get_site_data() override {
		return sites;
	}

	void populate_sites_machine_information(site& site) override {
		std::copy(machines.cbegin(), machines.cend(), std::back_inserter(site.machines));
	}

	void populate_channel_information_for_a_machine(machine& machine) override {
		std::copy(channels.cbegin(), channels.cend(), std::back_inserter(machine.channels));
	}

	std::vector<site> sites;
	std::vector<machine> machines;
	std::vector<channel> channels;
};