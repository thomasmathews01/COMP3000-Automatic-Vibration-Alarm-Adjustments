#pragma once
#include <gmock/gmock.h>
#include <IClock.h>
#include <Statistics/IStatsCalcFactory.h>
#include <Calculators/IAlarmCalcFactory.h>
#include "IConfigurationAccess.h"

class MockAlarmCalcFactory : public IAlarmCalcFactory {
public:
    MOCK_METHOD3(construct_calculator, std::shared_ptr<IAlarmCalc>(std::shared_ptr<IDataAccess> data_access, std::shared_ptr<IAlarmStorage> alarm_access, const alarm_settings_t&
    alarm_settings));
};

class MockStatsCalcFactory : public IStatsCalcFactory {
public:
    MOCK_METHOD4(get_statistics_calculator, std::shared_ptr<IAlarmStatistics>(int channel, int type, std::shared_ptr<IStatistics>  statistics_storage, std::shared_ptr<IDataAccess>
    data_storage));
};

class MockConfigurationAccess : public IConfigurationAccess {
public:
    MOCK_METHOD(std::vector<site>, get_site_data, (), (const, noexcept));
    MOCK_METHOD(std::vector<machine>, get_machines_for_site, (const site& site), (const, noexcept));
    MOCK_METHOD(std::vector<channel>, get_channel_information_for_machine, (const machine& machine), (const, noexcept));
    MOCK_METHOD(std::vector<channel>, get_all_channels, (), (const, noexcept));
	MOCK_METHOD(std::vector<data_type>, get_all_types, (), (const, noexcept));
    MOCK_METHOD(int, get_machine_id_from_channel_id, (int channel_id), (const, noexcept));
};

class MockDataAccess : public IDataAccess {
public:
    MOCK_METHOD((std::vector<std::pair<time_point_t, float>>), get_data, (int channel, int type, time_point_t start, time_point_t end), (const, noexcept));
    MOCK_METHOD((std::pair<time_point_t, float>), get_last_data_point_before, (int channel, int type, time_point_t time), (const, noexcept));
    MOCK_METHOD(std::vector<float>, get_data_points_only, (int channel, int type, time_point_t start, time_point_t end), (const, noexcept));
    MOCK_METHOD((std::vector<std::pair<int, std::string>>), get_data_types_available_for_channel, (int channel_id), (const, noexcept));
    MOCK_METHOD(std::vector<int>, get_all_data_types, (), (const, noexcept));
    MOCK_METHOD(time_point_t, get_earliest_data_point_for_machine, (int machine_id), (const, noexcept));
    MOCK_METHOD(time_point_t, get_latest_data_point_for_machine, (int machine_id), (const, noexcept));
};

class MockClock : public IClock {
public:
    MOCK_METHOD(time_point_t, get_current_time, (), (const, noexcept));
};

class MockStatistics : public IStatistics {
public:
    MOCK_METHOD2(get_last_statistics_calculation, statistics_point_t(int channel, int type));
    MOCK_METHOD3(update_last_statistics_calculation, void(int channel, int type, const statistics_point_t& new_values));

};

class MockAlarmStatistics : public IAlarmStatistics {
public:
    MOCK_METHOD(void, update, (const time_point_t& time), (const, noexcept));
};

class MockAlarmCalc : public IAlarmCalc {
public:
    MOCK_METHOD(void, update, (const time_point_t& time));
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
