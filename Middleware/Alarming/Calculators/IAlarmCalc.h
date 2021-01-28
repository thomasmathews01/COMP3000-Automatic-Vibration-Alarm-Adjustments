#pragma once

#include <types.h>
#include <alarmTypes.h>

class IAlarmCalc {
public:
	virtual void update_alarm_state_at(const time_point_t& time) = 0;
};


