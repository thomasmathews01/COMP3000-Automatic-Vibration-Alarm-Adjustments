#pragma once

#include <mutex>
#include "IDatabase.h"
#include "Helpers/sqlite3pp.h"
#include "IDatabaseFactory.h"

class Database : public IDatabase {
public:
	explicit Database(std::shared_ptr<IDatabaseFactory> factory);
	~Database() override = default;

	std::vector<site> get_site_data() final;
	void populate_sites_machine_information(site& site) final;
	void populate_channel_information_for_a_machine(machine& machine) final;
	std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) final;
	std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) final;
	int get_machine_id_from_channel_id(int channel_id) final;
	time_point_t get_earliest_data_point_for_machine(int machine_id) final;
	std::vector<state_change_t> get_state_changes_for_machine(int machine_id) final;
	std::vector<alarm_activation_t> get_activations_for_machine(int machine_id) final;
	std::vector<alarm_settings_t> get_alarm_settings_for_machine(int machine_id) final;
	bool update_alarm_setting(const alarm_settings_t& new_setting) final;
	std::vector<automatic_alarm_level_history_point_t> get_alarm_level_history(int channel_id, int type_id) final;
	void add_new_state_period(int machine_id, state_period_t state_period) final;
	void add_alarm_level_history_item(const time_point_t& occurence, const alarm_settings_t& associated_alarm, double new_level) final;

private:
	std::shared_ptr<IDatabaseFactory> factory;

	//std::mutex database_access_mutex;
	sqlite3pp::database database;
};


