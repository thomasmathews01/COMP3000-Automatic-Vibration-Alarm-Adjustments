#include "BasicStatisticsGenerator.h"
#include <range/v3/all.hpp>

using namespace ranges;

namespace
{
	template<class T>
	double calculate_mean(T&& values) {
		if (values.empty())
			return 0.0;

		return accumulate(values, 0.0) / values.size();
	}

	auto get_difference_from(double value) {
		return [value](const double x) { return x - value; };
	}

	template<class T>
	double calculate_std_deviation(T&& values, double mean) {
		auto squared_differences = values | views::transform(get_difference_from(mean)) | views::transform([](const double x) { return x * x; });
		auto sum_of_squared_differences = accumulate(squared_differences, 0.0);

		return sqrt(sum_of_squared_differences);
	}

	template<class T>
	requires ranges::range<T>
	std::pair<double, double> get_mean_and_standard_deviation(T&& values) {
		const auto mean = calculate_mean(values);
		const auto std_deviation = calculate_std_deviation(values, mean);

		return {mean, std_deviation};
	}
}


void BasicStatisticsGenerator::start() {

}

void BasicStatisticsGenerator::update() {
	std::for_each(endpoints.begin(), endpoints.end(), [this](const auto& x) { update_endpoint(x); });
}

BasicStatisticsGenerator::unique_endpoint BasicStatisticsGenerator::update_endpoint(BasicStatisticsGenerator::unique_endpoint& endpoint) {
	const auto current_time = time_point_t(100s);

	const auto points_since_last_change = database->get_data(endpoint.channel, endpoint.type, endpoint.last_update_time, current_time);
	const auto[mean, std_dev] = get_mean_and_standard_deviation(points_since_last_change | views::values);

	endpoint.last_update_time = points_since_last_change.back().first; // TODO: Extract into struct with named members
	//endpoint.mean = merge_means(endpoint.mean, endpoint.) // Introduce a smaple count so that they can be weighted and merged properly.
	//endpoint.standard_deviation = merge_standard_deviations(endpoint.standard_deviation, endpoint.)

}


