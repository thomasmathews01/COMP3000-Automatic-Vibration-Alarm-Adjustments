#pragma once
#include "IAlarmStorage.h"
#include "Helpers/sqlite3pp.h"
#include "IDatabaseFactory.h"

class AlarmStorage : public IAlarmStorage {
public:
    explicit AlarmStorage(const std::shared_ptr<IDatabaseFactory>& factory) : database(factory->get_database()) {}

    std::vector<alarm_settings_t> get_all_alarm_settings() override;
    std::vector<alarm_settings_t> get_alarm_settings_for_machine(int machine_id) override;
    bool update_alarm_setting(const alarm_settings_t& new_setting) override;
    alarm_settings_t get_updated_alarm_settings(const alarm_settings_t& old_settings) override;
    std::vector<alarm_activation_t> get_activations_for_machine(int machine_id) override;
    void add_alarm_activation(const alarm_activation_t& activation) override;
    alarm_state_t get_alarm_state_of_site(const site& site) override;
    alarm_state_t get_alarm_state_of_machine(const machine& machine) override;
    alarm_state_t get_alarm_state_of_channel(const channel& channel) override;
    alarm_state_t get_alarm_state_of_alarm(const alarm_settings_t& alarm) override;
    std::vector<alarm_level_history_point> get_alarm_level_history(const alarm_settings_t& associated_alarm) override;
    void add_alarm_level_history_item(const time_point_t& occurence, const alarm_settings_t& associated_alarm, double new_level) override;

private:
    std::shared_ptr<sqlite3pp::database> database; // This is a shared pointer for testing purposes, but should never be accessed by anyone other than this class, with the mutex, unless they take ownership whilst they use it, and we can't access it in that period.
};
