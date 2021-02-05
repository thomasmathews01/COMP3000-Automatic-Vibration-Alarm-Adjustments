#pragma once

#include "IAlarmCalc.h"
#include <IDatabase.h>
#include <memory>

class AlarmCalc : public IAlarmCalc {
public:
	AlarmCalc(std::shared_ptr<IDataAccess> data_access, std::shared_ptr<IAlarmStorage> alarm_access, const alarm_settings_t&	alarm_settings);

	void update(const time_point_t& time) override;
private:
	std::shared_ptr<IDataAccess> data_access;
	std::shared_ptr<IAlarmStorage> alarm_access;
	alarm_settings_t alarm_settings;

	std::pair<alarm_state_t, time_point_t> get_next_state(const time_point_t& time) const noexcept;
	void add_state_change(const alarm_state_t& new_state, const time_point_t& time_of_state_change) const noexcept;
};


