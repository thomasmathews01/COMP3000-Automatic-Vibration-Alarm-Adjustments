#pragma once
#include "StubDataAccess.h"
#include <range/v3/all.hpp>

using namespace ranges;

std::vector<std::pair<time_point_t, float>> StubDataAccess::get_data(int channel, int type, time_point_t start, time_point_t end) {
	return data;
}

std::vector<float> StubDataAccess::get_data_points_only(int channel, int type, time_point_t start, time_point_t end) {
	return data | views::values | to<std::vector>();
}

time_point_t StubDataAccess::get_earliest_data_point_for_machine(int machine_id) {
	return ranges::min(data | views::keys);
}

std::vector<std::pair<int, std::string>> StubDataAccess::get_data_types_available_for_channel(int channel_id) {
	return data_types;
}

std::pair<time_point_t, float> StubDataAccess::get_last_data_point_before(int channel, int type, time_point_t time) {
	return data.back();
}

std::vector<int> StubDataAccess::get_all_data_types() {
    return data_types | views::keys | to<std::vector>();
}

