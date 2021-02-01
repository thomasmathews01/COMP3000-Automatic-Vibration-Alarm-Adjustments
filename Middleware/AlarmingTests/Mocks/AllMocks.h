#pragma once

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
    MOCK_METHOD0(get_site_data, std::vector<site>(void));
    MOCK_METHOD1(get_machines_for_site, std::vector<machine>(const site& site));
    MOCK_METHOD1(get_channel_information_for_machine, std::vector<channel>(const machine& machine));
    MOCK_METHOD0(get_all_channels, std::vector<channel>(void));
    MOCK_METHOD1(get_machine_id_from_channel_id, int(int channel_id));
};

class MockDataAccess : public IDataAccess {
public:
    MOCK_METHOD4(get_data, std::vector<std::pair<time_point_t, float>>(int channel, int type, time_point_t start, time_point_t end));
    MOCK_METHOD3(get_last_data_point_before, std::pair<time_point_t, float>(int channel, int type, time_point_t time));
    MOCK_METHOD4(get_data_points_only, std::vector<float>(int channel, int type, time_point_t start, time_point_t end));
    MOCK_METHOD1(get_data_types_available_for_channel, std::vector<std::pair<int, std::string>>(int channel_id));
    MOCK_METHOD0(get_all_data_types, std::vector<int>(void));
    MOCK_METHOD1(get_earliest_data_point_for_machine, time_point_t(int machine_id));
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
    MOCK_METHOD(void, update, (const time_point_t& time));
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
