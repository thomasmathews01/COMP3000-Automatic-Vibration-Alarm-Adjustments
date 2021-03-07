#include "DataStorage.h"
#include <algorithm>

using namespace std::string_literals;

namespace {
    int seconds_since_epoch(const time_point_t t) noexcept {
        return duration_cast<seconds>(t.time_since_epoch()).count();
    }

    std::string seconds_since_epoch_str(const time_point_t t) noexcept {
        return std::to_string(seconds_since_epoch(t));
    }
}

std::vector<std::pair<time_point_t, float>>
DataStorage::get_data(int channel, int type, time_point_t start, time_point_t finish) const noexcept {
    const auto statement = "SELECT time_since_epoch, value FROM data"s +
                           " WHERE type_id =  " + std::to_string(type) +
                           " AND time_since_epoch BETWEEN " + seconds_since_epoch_str(start) + " AND " +
                           std::to_string(seconds_since_epoch(finish)) +
                           " AND channel_id = " + std::to_string(channel) + " ";

    std::vector<std::pair<time_point_t, float>> data;

    const auto results = database->execute(statement);
    const auto results_values = results.vector<std::tuple<int, float>>();

    for (const auto[epoch_time, value] : results_values)
    	data.emplace_back(time_point_t(seconds(epoch_time)), value);

    return data;
}

std::vector<std::pair<int, std::string>> DataStorage::get_data_types_available_for_channel(int channel_id) const noexcept {
    constexpr auto statement = "SELECT DISTINCT type_id, type_name FROM types";

    const auto result = database->execute(statement);
    const auto result_values = result.vector<std::tuple<int, std::string>>();

	std::vector<std::pair<int, std::string>> types;

	for (const auto[type_id, type_name] : result_values) {
		types.emplace_back(type_id, type_name);
	}

	return types;
}

time_point_t DataStorage::get_earliest_data_point_for_machine(int machine_id) const noexcept {
    const auto selection_string =
            "SELECT min(data.time_since_epoch) from data inner join channels on data.channel_id = channels.channel_id "
            "inner join machines on channels.machine_id = machines.machine_id where machines.machine_id = " +
            std::to_string(machine_id);

    const auto result = database->execute(selection_string);
    const auto result_values = result.vector<int>();
    if (!result_values.empty())
		return time_point_t(seconds(result_values.front()));

    return time_point_t(0s);
}

time_point_t DataStorage::get_latest_data_point_for_machine(int machine_id) const noexcept {
    const auto selection_string =
            "SELECT max(data.time_since_epoch) from data inner join channels on data.channel_id = channels.channel_id "
            "inner join machines on channels.machine_id = machines.machine_id where machines.machine_id = " +
            std::to_string(machine_id);

    const auto results = database->execute(selection_string);
    const auto result_values = results.vector<int>();
	if (!result_values.empty())
		return std::min(time_point_t(seconds(result_values.front())), clock->get_current_time());

    return time_point_t(0s);
}

std::pair<time_point_t, float> DataStorage::get_last_data_point_before(int channel, int type, time_point_t time) const noexcept {
    const auto statement = "SELECT time_since_epoch, value FROM data"s +
                           " WHERE type_id =  " + std::to_string(type) +
                           " AND time_since_epoch <= " + seconds_since_epoch_str(time) +
                           " AND channel_id = " + std::to_string(channel) +
                           " ORDER BY time_since_epoch DESC" +
                           " LIMIT 1";

    const auto result = database->execute(statement);
    const auto result_values = result.vector<std::tuple<int, float>>();
    if (!result_values.empty()) {
		const auto[epoch_time, value] = result_values.front();

		return std::make_pair(time_point_t(seconds(epoch_time)), value);
    }

    throw std::invalid_argument("Something has gone terribly wrong finding the last data point before a given time");
}
std::vector<float> DataStorage::get_data_points_only(int channel, int type, time_point_t start, time_point_t finish) const noexcept {
    const auto statement = "SELECT value FROM data"s +
                           " WHERE type_id =  " + std::to_string(type) +
                           " AND channel_id = " + std::to_string(channel) +
                           " AND time_since_epoch BETWEEN " + seconds_since_epoch_str(start + 1s) + " AND " +
                           std::to_string(seconds_since_epoch(finish));

    const auto result = database->execute(statement);

    return result.vector<float>();
}

std::vector<int> DataStorage::get_all_data_types() const noexcept {
    const auto statement = "SELECT DISTINCT type_id FROM types";
    const auto result = database->execute(statement);

    return result.vector<int>();
}
