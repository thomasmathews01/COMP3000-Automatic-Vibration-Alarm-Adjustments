#pragma once
#include "IDatabase.h"
#include <map>
#include <iterator>

struct TestDatabase : public IDatabase {

	std::vector<site> get_site_data() final {
		return sites;
	}

	std::vector<machine> get_machines_for_site(const site& site) final {
		return machines;
	}

	void add_new_state_period(int machine_id, state_period_t state_period) final {

	}

	std::vector<alarm_activation_t> get_activations_for_machine(int machine_id) final {
		return std::vector<alarm_activation_t>();
	}

	std::vector<alarm_settings_t> get_alarm_settings_for_machine(int machine_id) final {
		return std::vector<alarm_settings_t>();
	}

	bool update_alarm_setting(const alarm_settings_t& new_setting) final {
		return false;
	}

	std::vector<alarm_level_history_point> get_alarm_level_history(const alarm_settings_t& associated_alarm) final {
		return std::vector<alarm_level_history_point>();
	}

	std::vector<channel> get_channel_information_for_machine(const machine& machine) final {
		return channels;
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

	void add_alarm_level_history_item(const time_point_t& occurence, const alarm_settings_t& associated_alarm, double new_level) final {

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