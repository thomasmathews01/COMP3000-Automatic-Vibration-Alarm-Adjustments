#pragma once
#include "types.h"

class IDataAccess {
public:
	[[nodiscard]] virtual std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) const noexcept = 0;
	[[nodiscard]] virtual std::pair<time_point_t, float> get_last_data_point_before(int channel, int type, time_point_t time) const noexcept = 0;
	[[nodiscard]] virtual std::vector<float> get_data_points_only(int channel, int type, time_point_t start, time_point_t end) const noexcept = 0; // TODO: Test
	[[nodiscard]] virtual std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) const noexcept = 0;
	[[nodiscard]] virtual std::vector<int> get_all_data_types() const noexcept = 0;
	[[nodiscard]] virtual time_point_t get_earliest_data_point_for_machine(int machine_id) const noexcept = 0;
	[[nodiscard]] virtual time_point_t get_latest_data_point_for_machine(int machine_id) const noexcept = 0;
};