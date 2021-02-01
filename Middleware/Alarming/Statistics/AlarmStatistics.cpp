#include "AlarmStatistics.h"
#include <utility>
#include <range/v3/all.hpp>

using namespace ranges;

void AlarmStatistics::update(const time_point_t& time) {
	auto calculation_cache = statistics_storage->get_last_statistics_calculation(channel, type);

	const auto new_data = data_storage->get_data(channel, type, calculation_cache.time_of_last_point_in_calculation, time);

	if (new_data.empty())
		return;

	const auto new_calculation_cache = update_statistics(calculation_cache, new_data);
	statistics_storage->update_last_statistics_calculation(channel, type, new_calculation_cache);
}

statistics_point_t AlarmStatistics::update_statistics(const statistics_point_t& cached_values, const std::vector<std::pair<time_point_t, float>>& new_data) {
	auto new_values = cached_values;

	for (const auto new_sample : new_data | views::values) // Wellford Algorithm
	{
		const double delta = new_sample - new_values.mean;

        new_values.mean += static_cast<float>(delta / ++new_values.sample_count);
        new_values.m2 += delta * (new_sample - new_values.mean);
        new_values.variance = static_cast<float>(new_values.m2 / new_values.sample_count);
	}

    new_values.time_of_last_point_in_calculation = max(new_data | views::keys);

	return new_values;
}


AlarmStatistics::AlarmStatistics(int channel, int type, std::shared_ptr<IStatistics> statistics_storage, std::shared_ptr<IDataAccess> data_storage)
	: channel(channel), type(type), statistics_storage(std::move(statistics_storage)), data_storage(std::move(data_storage)) {}
