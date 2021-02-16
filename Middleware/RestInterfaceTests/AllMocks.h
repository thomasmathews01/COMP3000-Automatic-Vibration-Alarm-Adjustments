#pragma once
#include <IConfigurationAccess.h>
#include <IAlarmStorage.h>

class MockConfigurationAccess : public IConfigurationAccess {
public:
	MOCK_METHOD(std::vector<site>, get_site_data, (), (const, noexcept));
	MOCK_METHOD(std::vector<machine>, get_machines_for_site, (const site& site), (const, noexcept));
	MOCK_METHOD(std::vector<channel>, get_channel_information_for_machine, (const machine& machine), (const, noexcept));
	MOCK_METHOD(std::vector<channel>, get_all_channels, (), (const, noexcept));
	MOCK_METHOD(int, get_machine_id_from_channel_id, (int channel_id), (const, noexcept));
};

class MockAlarmStorage : public IAlarmStorage {
public:
	// Alarm Settings
	MOCK_METHOD(std::vector<alarm_settings_t>, get_all_alarm_settings, ());
	MOCK_METHOD(std::vector<alarm_settings_t>, get_alarm_settings_for_machine, (int machine_id));
	MOCK_METHOD(bool, update_alarm_setting, (const alarm_settings_t& new_setting));
	MOCK_METHOD(alarm_settings_t, get_updated_alarm_settings, (const alarm_settings_t& old_settings));

	// Alarm firings
	MOCK_METHOD(std::vector<alarm_activation_t>, get_activations_for_machine, (int machine_id));
	MOCK_METHOD(void, add_alarm_activation, (const alarm_activation_t& activation));
	MOCK_METHOD(alarm_state_t, get_alarm_state_of_site, (const site& site));
	MOCK_METHOD(alarm_state_t, get_alarm_state_of_machine, (const machine& machine));
	MOCK_METHOD(alarm_state_t, get_alarm_state_of_channel, (const channel& channel));
	MOCK_METHOD(alarm_state_t, get_alarm_state_of_alarm, (const alarm_settings_t& alarm));

	// Alarm log
	MOCK_METHOD(std::vector<alarm_level_history_point>, get_alarm_level_history, (const alarm_settings_t& associated_alarm));
	MOCK_METHOD(void, add_alarm_level_history_item, (const time_point_t& occurence, const alarm_settings_t& associated_alarm, double new_level));
};
