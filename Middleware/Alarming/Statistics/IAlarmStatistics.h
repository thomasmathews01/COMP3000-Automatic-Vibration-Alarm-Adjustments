#pragma once
#include "types.h"

class IAlarmStatistics {
public:
	virtual void update(const time_point_t& time) = 0;
};

class EmptyAlarmStatistics : IAlarmStatistics {
public:
	void update(const time_point_t& time) override {}
};


