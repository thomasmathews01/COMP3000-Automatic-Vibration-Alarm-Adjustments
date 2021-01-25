#include "AlarmStatistics.h"
#include <utility>
#include <range/v3/all.hpp>

using namespace ranges;

void AlarmStatistics::update() {
	const auto current_time = clock->get_current_time();
	auto calculation_cache = statistics_storage->get_last_statistics_calculation(settings.channel_id, settings.type_id);

	const auto new_data = data_storage->get_data(settings.channel_id, settings.type_id, calculation_cache.time_of_last_point_in_calculation, current_time);

	if (new_data.empty())
		return;

	update_statistics(calculation_cache, new_data);
	statistics_storage->update_last_statistics_calculation(settings.channel_id, settings.type_id, calculation_cache);
}

void AlarmStatistics::update_statistics(statistics_point_t& cached_values, const std::vector<std::pair<time_point_t, float>>& new_data) { // Wellford Algorithm
	for (const auto new_sample : new_data | views::values)
	{
		const double delta = new_sample - cached_values.mean;

		cached_values.mean += static_cast<float>(delta / ++cached_values.sample_count);
		cached_values.m2 += delta * (new_sample - cached_values.mean);
		cached_values.variance = static_cast<float>(cached_values.m2 / cached_values.sample_count);
	}

	cached_values.time_of_last_point_in_calculation = new_data.back().first;
}


AlarmStatistics::AlarmStatistics(alarm_settings_t settings, std::shared_ptr<IStatistics> statistics_storage, std::shared_ptr<IDataAccess> data_storage, std::shared_ptr<IClock> clock)
	: settings(settings), statistics_storage(std::move(statistics_storage)), data_storage(std::move(data_storage)), clock(std::move(clock)) {}
