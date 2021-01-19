#pragma once

#include <string>
#include <vector>
#include "../Types/stateTypes.h"
#include "../Types/alarmTypes.h"

class IDatabase {
public:
	virtual std::vector<site> get_site_data() = 0;
	virtual std::vector<machine> get_machines_for_site(const site& site) = 0;
	virtual std::vector<channel> get_channel_information_for_machine(const machine& machine) = 0;
	virtual std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) = 0;
	virtual std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) = 0;
	virtual int get_machine_id_from_channel_id(int channel_id) = 0;
	virtual void add_new_state_period(int machine_id, state_period_t state_period) = 0;
	virtual time_point_t get_earliest_data_point_for_machine(int machine_id) = 0;
	virtual std::vector<state_change_t> get_state_changes_for_machine(int machine_id) = 0;
	virtual std::vector<alarm_activation_t> get_activations_for_machine(int machine_id) = 0;
	virtual std::vector<alarm_settings_t> get_alarm_settings_for_machine(int machine_id) = 0;
	virtual bool update_alarm_setting(const alarm_settings_t& new_setting) = 0;
	virtual std::vector<alarm_level_history_point> get_alarm_level_history(const alarm_settings_t& associated_alarm) = 0;
	virtual void add_alarm_level_history_item(const time_point_t& occurence, const alarm_settings_t& associated_alarm, double new_level) = 0;
	virtual void add_alarm_activation(const alarm_activation_t& activation) = 0;
	virtual ~IDatabase() = default;
};


