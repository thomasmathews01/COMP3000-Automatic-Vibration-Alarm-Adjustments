#pragma once

#include "IDatabase.h"

struct TestDatabase : public IDatabase {

	std::vector<site> get_site_data() override {
		return sites;
	}

	void populate_sites_machine_information(site& site) final {
		std::copy(machines.cbegin(), machines.cend(), std::back_inserter(site.machines));
	}

	void populate_channel_information_for_a_machine(machine& machine) final {
		std::copy(channels.cbegin(), channels.cend(), std::back_inserter(machine.channels));
	}

	std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) final {
		return data;
	}

	virtual std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) final {
		return data_types;
	}

	std::vector<site> sites;
	std::vector<machine> machines;
	std::vector<channel> channels;
	std::vector<std::pair<time_point_t, float>> data;
	std::vector<std::pair<int, std::string>> data_types;
};