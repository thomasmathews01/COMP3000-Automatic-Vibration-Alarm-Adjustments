#pragma once

#include "alarmTypes.h"

class IAlarmCalculator {
public:
	[[nodiscard]] virtual alarm_state_t get_alarm_state_at(const time_point_t& time) const noexcept = 0;
	[[nodiscard]] virtual alarm_settings_t get_alarm_settings() const noexcept = 0;
};


