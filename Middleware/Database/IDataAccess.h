#pragma once
#include "types.h"

class IDataAccess {
public:
	virtual std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) = 0;
	virtual std::vector<float> get_data_points_only(int channel, int type, time_point_t start, time_point_t end) = 0; // TODO: Test
	virtual std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) = 0;
	virtual time_point_t get_earliest_data_point_for_machine(int machine_id) = 0;
};