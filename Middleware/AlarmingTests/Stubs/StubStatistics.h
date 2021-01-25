#pragma once
#include "IStatistics.h"

class StubStatistics : public IStatistics {
public:
	virtual statistics_point_t get_last_statistics_calculation(int channel, int type) override {
		return point;
	}

	virtual void update_last_statistics_calculation(int channel, int type, const statistics_point_t& new_values) override {
		point = new_values;
	}

	statistics_point_t point;
};