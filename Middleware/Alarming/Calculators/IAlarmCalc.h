#pragma once

#include <types.h>
#include <alarmTypes.h>

class IAlarmCalc {
public:
	virtual void update(const time_point_t& time) = 0;
};


