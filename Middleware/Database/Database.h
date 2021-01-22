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
	std::vector<machine> get_machines_for_site(const site& site) final;
	std::vector<channel> get_channel_information_for_machine(const machine& machine) final;

	std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) final;
	std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) final;

	std::vector<alarm_level_history_point> get_alarm_level_history(const alarm_settings_t& associated_alarm) final;
	void add_alarm_level_history_item(const time_point_t& occurence, const alarm_settings_t& associated_alarm, double new_level) final;

	void add_alarm_activation(const alarm_activation_t& activation) final;
	std::vector<alarm_activation_t> get_activations_for_machine(int machine_id) final;

	int get_machine_id_from_channel_id(int channel_id) final;
	time_point_t get_earliest_data_point_for_machine(int machine_id) final;

	std::vector<alarm_settings_t> get_all_alarm_settings() final; // not tested
	std::vector<alarm_settings_t> get_alarm_settings_for_machine(int machine_id) final;
	bool update_alarm_setting(const alarm_settings_t& new_setting) final; // Tested up to here.

	void add_new_state_period(int machine_id, state_period_t state_period) final;
	std::vector<state_change_t> get_state_changes_for_machine(int machine_id) final;

	alarm_state_t get_alarm_state_of_site(const site& site) final;
	alarm_state_t get_alarm_state_of_machine(const machine& machine) final;
	alarm_state_t get_alarm_state_of_channel(const channel& channel) final;

	virtual void add_state(int machine_id, const state_t& state) override;
	virtual void remove_state(int machine_id, int state_id) override;
	virtual std::vector<state_t> get_states_for_machine(int machine_id) override;
	virtual alarm_state_t get_alarm_state_of_alarm(const alarm_settings_t& alarm) override;

private:
	std::shared_ptr<IDatabaseFactory> factory;

	std::mutex database_access_mutex;
	std::shared_ptr<sqlite3pp::database> database; // This is a shared pointer for testing purposes, but should never be accessed by anyone other than this class, with the mutex, unless they take ownership whilst they use it, and we can't access it in that period.

	std::optional<int> get_state_at_time(const time_point_t& time, int machine_id);
	std::optional<time_point_t> get_time_of_last_state_change_before(const time_point_t& time, int machine_id);
	void add_new_state_changes_for_machine(const std::vector<state_change_t>& changes, int machine_id);
};


