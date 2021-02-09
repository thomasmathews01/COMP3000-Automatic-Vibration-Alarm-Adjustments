#pragma once
#include <IDataAccess.h>
#include <IConfigurationAccess.h>
#include <range/v3/all.hpp>
#include <cassert>
#include <utility>

/// Class for aggregating data into feature collections
//
// ML model requires points in a many dimensional plane, and is
// difficult to directly break elements out of to test due to the
// templating, which this exists to break
/// \tparam N Dimensions within data
template<int N>
class MLDataAggregator {
public:
	using individual_point_t = std::array<float, N>;
	using feature_set_t = std::vector<std::pair<time_point_t, individual_point_t>>;
	MLDataAggregator(std::shared_ptr<IDataAccess>  data_storage, std::shared_ptr<IConfigurationAccess> config_storage) : data_storage(std::move(data_storage)), config_storage(std::move(config_storage)) {}

private:
	constexpr static auto feature_time_comparison = [](const std::pair<time_point_t, individual_point_t>& pair, const time_point_t& time)
	{
		return pair.first < time;
	};

	/// @brief Returns all channel type combinations on the given machine that have attached data.
	/// @param machine_id The machine to search on
	/// @return Collection of pairs of channel to type
	auto get_channel_type_pairs(int machine_id) {
		std::vector<std::pair<int , int>> channel_type_pairs;

		for (auto& channel : config_storage->get_channel_information_for_machine(machine(machine_id))) {
			const auto all_types = data_storage->get_data_types_available_for_channel(channel.id);
			for (auto& [type_id, type_name] : all_types)
				channel_type_pairs.emplace_back(std::make_pair<int, int>(static_cast<int>(channel.id), static_cast<int>(type_id))); // TODO: This is clearly ridiculous because the compiler was getting upset and needs tidying
		}

		return channel_type_pairs;
	}

	/// Get the data point at a given time, and insert a new one if not there already
	/// \param data_points A time-sorted collection of data points
	/// \param time The time of the data point to find/create
	/// \return non-const iterator pointing at the value for the given time
	auto get_data_point_at_time(feature_set_t& data_points, const time_point_t& time) {
		auto insert_location = std::lower_bound(data_points.begin(), data_points.end(), time, feature_time_comparison); // Binary search to find location where the time should be
		if (insert_location == data_points.end())
		{
			data_points.push_back({time, {0}});
			return data_points.begin() + data_points.size() -1; // Ths was the last inserted point
		}

		if (insert_location->first != time) // If there isn't a point with this time, insert one in sorted position.
			insert_location = data_points.insert(insert_location, {time, {0}});

		return insert_location;
	}
public:
	/// @brief Collates all data for a machine with each source as its own dimension
	/// @param machine_id The machine to search on
	/// @param time The upper time limit, will only add data points before this time.
	/// @return Collection of pairs of time stamp to feature array
	feature_set_t get_data_before_time(int machine_id, time_point_t time){
		const auto channel_type_pairs = get_channel_type_pairs(machine_id);
		assert(channel_type_pairs.size() <= N); // If this falls over in 'production' then the assumption made about the structure of the data files was wrong. Which means an awful lot of existing code is actually invalid. and the model is missing data

		feature_set_t data_points;
		for (int i = 0; i < channel_type_pairs.size() && i < N; ++i)
			for (const auto&[time, value] : data_storage->get_data(channel_type_pairs.at(i).first, channel_type_pairs.at(i).second, time_point_t(0s), time))
				get_data_point_at_time(data_points, time)->second.at(i) = value;

		return data_points;
	}

private:
	std::shared_ptr<IDataAccess> data_storage;
	std::shared_ptr<IConfigurationAccess> config_storage;
};
