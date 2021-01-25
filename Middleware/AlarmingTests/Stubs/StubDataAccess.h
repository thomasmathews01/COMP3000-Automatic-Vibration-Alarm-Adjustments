#pragma once
#include "IDataAccess.h"

class StubDataAccess : public IDataAccess {
public:
	std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) override;

	std::vector<float> get_data_points_only(int channel, int type, time_point_t start, time_point_t end) override;

	std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) override;

	time_point_t get_earliest_data_point_for_machine(int machine_id) override;

	std::vector<std::pair<time_point_t, float>> data;
	std::vector<std::pair<int, std::string>> data_types;
};
