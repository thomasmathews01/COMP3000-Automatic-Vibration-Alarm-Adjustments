#pragma once
#include "alarmTypes.h"

class IStatistics {
public:
	virtual statistics_point_t get_last_statistics_calculation(int channel, int type) = 0;
	virtual void update_last_statistics_calculation(int channel, int type, const statistics_point_t& new_values) = 0;
};