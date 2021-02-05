#pragma once
#include "types.h"

class IAlarmStatistics {
public:
	virtual void update(const time_point_t& time) const noexcept = 0;
};


