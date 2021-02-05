#include "AlarmStatistics.h"
#include <utility>
#include <range/v3/all.hpp>

using namespace ranges;

namespace {
	constexpr void perform_wellford_update(const float sample, statistics_point_t& cached_values) noexcept {
		const double delta = sample - cached_values.mean;

		cached_values.mean += static_cast<float>(delta / ++cached_values.sample_count);
		cached_values.m2 += delta * (sample - cached_values.mean);
		cached_values.variance = static_cast<float>(cached_values.m2 / cached_values.sample_count);
	}
}


void AlarmStatistics::update(const time_point_t& time) const noexcept {
	const auto calculation_cache = statistics_storage->get_last_statistics_calculation(channel, type);

	const auto new_data = data_storage->get_data(channel, type, calculation_cache.time_of_last_point_in_calculation, time);
	const auto new_calculation_cache = update_statistics(calculation_cache, new_data);

	statistics_storage->update_last_statistics_calculation(channel, type, new_calculation_cache);
}

statistics_point_t AlarmStatistics::update_statistics(const statistics_point_t& cached_values, const std::vector<sample_t>& new_data) noexcept {
	auto new_cache = cached_values;

	for (const auto new_sample : new_data | views::values)
		perform_wellford_update(new_sample, new_cache);

    new_cache.time_of_last_point_in_calculation = max(new_data | views::keys);

	return new_cache;
}


AlarmStatistics::AlarmStatistics(int channel, int type, std::shared_ptr<IStatistics> statistics_storage, std::shared_ptr<IDataAccess> data_storage)
	: channel(channel), type(type), statistics_storage(std::move(statistics_storage)), data_storage(std::move(data_storage)) {}
