#pragma once

#include <string>
#include <vector>
#include "../REST Interface/Model/stateTypes.h"


class IDatabase {
public:
	virtual std::vector<site> get_site_data() = 0;
	virtual void populate_sites_machine_information(site& site) = 0;
	virtual void populate_channel_information_for_a_machine(machine& machine) = 0;
	virtual std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) = 0;
	virtual std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) = 0;
	virtual int get_machine_id_from_channel_id(int channel_id) = 0;
	virtual void update_state_changes_for_machine(int channel_id) = 0;
	virtual time_point_t get_earliest_data_point_for_machine(int machine_id) = 0;
	virtual std::vector<state_change_t> get_state_changes_for_machine(int machine_id) = 0;
};


