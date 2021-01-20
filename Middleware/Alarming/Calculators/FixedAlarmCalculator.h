#pragma once

#include <alarmTypes.h>
#include "IAlarmCalculator.h"
#include "IDatabase.h"

class FixedAlarmCalculator : public IAlarmCalculator {
public:
	FixedAlarmCalculator(const alarm_settings_t& alarm_settings, std::shared_ptr<IDatabase> database);

	[[nodiscard]] alarm_state_t get_alarm_state_at(const time_point_t& time) const noexcept final;
	[[nodiscard]] alarm_settings_t get_alarm_settings() const noexcept override;

private:
	alarm_settings_t alarm_settings;
	std::shared_ptr<IDatabase> database;
};


