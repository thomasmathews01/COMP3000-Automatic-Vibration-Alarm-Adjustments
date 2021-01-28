#include "alarmTypes.h"

class IAlarmStorage {
public:
	// Alarm Settings
	virtual std::vector<alarm_settings_t> get_all_alarm_settings() = 0;
	virtual std::vector<alarm_settings_t> get_alarm_settings_for_machine(int machine_id) = 0;
	virtual bool update_alarm_setting(const alarm_settings_t& new_setting) = 0;
	virtual alarm_settings_t get_updated_alarm_settings(const alarm_settings_t& old_settings) = 0;

	// Alarm firings
	virtual std::vector<alarm_activation_t> get_activations_for_machine(int machine_id) = 0;
	virtual void add_alarm_activation(const alarm_activation_t& activation) = 0;
	virtual alarm_state_t get_alarm_state_of_site(const site& site) = 0;
	virtual alarm_state_t get_alarm_state_of_machine(const machine& machine) = 0;
	virtual alarm_state_t get_alarm_state_of_channel(const channel& channel) = 0;
	virtual alarm_state_t get_alarm_state_of_alarm(const alarm_settings_t& alarm) = 0;

	// Alarm log
	virtual std::vector<alarm_level_history_point> get_alarm_level_history(const alarm_settings_t& associated_alarm) = 0;
	virtual void add_alarm_level_history_item(const time_point_t& occurence, const alarm_settings_t& associated_alarm, double new_level) = 0;
};