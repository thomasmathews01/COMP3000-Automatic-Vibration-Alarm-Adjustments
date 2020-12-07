#pragma once
#include "IDatabase.h"
#include <map>
#include <iterator>

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

	std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) final {
		return data_types;
	}

	int get_machine_id_from_channel_id(int channel_id) final {
		if (channels_to_machines.count(channel_id))
			return channels_to_machines.at(channel_id);

		return 0;
	}

	time_point_t get_earliest_data_point_for_machine(int machine_id) final {
		return earliest_data_point;
	}

	std::vector<state_change_t> get_state_changes_for_machine(int machine_id) final {
		return state_changes;
	}

	std::vector<site> sites;
	std::vector<machine> machines;
	std::vector<channel> channels;
	std::vector<std::pair<time_point_t, float>> data;
	std::vector<std::pair<int, std::string>> data_types;
	time_point_t earliest_data_point;
	std::vector<state_change_t> state_changes;
	std::map<int, int> channels_to_machines;
};