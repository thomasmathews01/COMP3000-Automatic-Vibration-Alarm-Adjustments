#pragma once

#include "IAlarmCalc.h"
#include <IDatabase.h>
#include <memory>

class AlarmCalc : public IAlarmCalc {
public:
	AlarmCalc(std::shared_ptr<IDatabase> database, const alarm_settings_t& alarm_settings);

	void update_alarm_state_at(const time_point_t& time) override;
private:
	std::shared_ptr<IDatabase> database;
	alarm_settings_t alarm_settings;

	std::pair<alarm_state_t, time_point_t> get_next_state(const time_point_t& time);
	std::pair<alarm_state_t, time_point_t> get_state_of_alarm_fixed(const time_point_t& time);
	void add_state_change(const alarm_state_t& new_state, const time_point_t& time_of_state_change);
};


