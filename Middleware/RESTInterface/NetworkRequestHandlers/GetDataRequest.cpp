#include "GetDataRequest.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <cmath>


using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;

constexpr auto maximum_number_of_points = 5000;
using points_collection_t = std::array<std::pair<time_point_t, float>, maximum_number_of_points>;
using decimated_points = std::pair<float, points_collection_t>; // Decimation rate, and the decimated points.

std::string convert_data_points_to_json(const decimated_points& data) {
	StringBuffer buff;
	PrettyWriter<StringBuffer> writer(buff);
	writer.StartObject();

	writer.Key("decimation_factor");
	writer.Double(data.first);
	writer.Key("data_points");
	writer.StartArray();

	for (const auto& point : data.second) {
		writer.StartObject();
		writer.Key("time");
		writer.Int(duration_cast<seconds>(point.first.time_since_epoch()).count());
		writer.Key("value");
		writer.Double(point.second);
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return buff.GetString();
}

template<class in_it, class out_it>
out_it copy_every_n( in_it b, in_it e, out_it r, size_t n) { // TODO: Move to a utils class
	for (size_t i=distance(b,e)/n; i--; advance (b,n))
		*r++ = *b;
	return r;
}

/**
 * Summary: Provides integer decimation of the input data, to the integer required to fit the points in a maximum 5000 element data structure.
 * If frequently called on data sets only slightly larger than the decimation rate this will result in lots of points lost unnecessarily.
 * This is not done with respect to time, but blindly.
 * @param full_data_set All points representing the range to be decimated
 * @return decimation factor, where a factor n indicated we kept 1 in every 1 points from the original set
 */
decimated_points decimate_data(const std::vector<std::pair<time_point_t, float>>& full_data_set) {
	decimated_points result;
	result.first = (int) std::ceilf(full_data_set.size() / (float) maximum_number_of_points); // This represents the integer gap between valid points.

	std::fill(result.second.begin(), result.second.end(), std::make_pair(time_point_t(), -1 * std::numeric_limits<float>::max()));
	copy_every_n(full_data_set.cbegin(), full_data_set.cend(), result.second.begin(), result.first);

	return result;
}

/**
 *
 * @param request crow request containing enough information to identify data points (start, end, channel & type)
 * @param database Database instance to retrieve the data from
 * @return JSON structure, as a string, representing up to 5000 points in the given range. Integer decimation performed to fit the points in this range.
 */
std::string GetDataRequest::get_data_points(const crow::request& request, const std::shared_ptr<IDatabase>& database) {
	const auto channel = std::stoi(request.url_params.get("channel")); // TODO: sensible error handling for malformed requests.
	const auto type = std::stoi(request.url_params.get("type")); // TODO: Logging for easier understanding of what has gone wrong.

	const auto start_seconds_since_epoch = std::stoi(request.url_params.get("start"));
	const auto start_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(start_seconds_since_epoch));

	const auto end_seconds_since_epoch = std::stoi(request.url_params.get("end"));
	const auto end_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(end_seconds_since_epoch));

	const auto data_points = database->get_data(channel, type, start_time, end_time);
	const auto decimated_data = decimate_data(data_points);

	return convert_data_points_to_json(decimated_data);
}
/*

namespace testing
{
	std::vector<std::pair<time_point_t, float>> get_test_collection(int sample_count) {
		auto collection = std::vector<std::pair<time_point_t, float>>();
		collection.reserve(sample_count);

		for (int i = 0; i < sample_count; ++i)
			collection.emplace_back(time_point_t() + std::chrono::seconds(i), (float) i);

		return collection;
	}

	TEST_CASE ("Decimation") {
			SUBCASE("Full set of 5000 points is returned in original condition") {
			const auto data = get_test_collection(maximum_number_of_points);

			const auto decimation_result = decimate_data(data);

			const auto match = std::equal(decimation_result.second.cbegin(), decimation_result.second.cend(), data.cbegin());
				CHECK(match);
		}

			SUBCASE("Full set of 5000 points returns a valid decimation factor") {
			const auto data = get_test_collection(maximum_number_of_points);

			const auto decimation_result = decimate_data(data);
				CHECK_EQ(decimation_result.first, 1);
		}

			SUBCASE("Small set is not decimated.") {
			const auto data = get_test_collection(maximum_number_of_points / 10);

			const auto decimation_result = decimate_data(data);

			const auto match = std::equal(data.cbegin(), data.cend(), decimation_result.second.cbegin());
				CHECK(match);
		}

			SUBCASE("Handles 2x decimation as expected") {
			const auto data = get_test_collection(2 * maximum_number_of_points);

			const auto[decimation_factor, decimated_data] = decimate_data(data);
				CHECK_EQ(decimation_factor, 2);
				CHECK(std::all_of(decimated_data.cbegin(), decimated_data.cend(), [](const std::pair<time_point_t, float>& pair) { return (int) pair.second % 2 == 0; }));
		}
			SUBCASE("Handles Non-Integer decimation (1.5x)") {
			const auto data = get_test_collection(1.5 * maximum_number_of_points);

			const auto[decimation_factor, decimated_data] = decimate_data(data);
				CHECK_EQ(decimation_factor, 2);
				CHECK(std::all_of(decimated_data.cbegin(), decimated_data.cend(), [](const std::pair<time_point_t, float>& pair) { return (int) pair.second % 2 == 0; }));
				CHECK_EQ(0.75 * maximum_number_of_points, std::count_if(decimated_data.cbegin(), decimated_data.cend(), [](const auto& pair) { return pair.second != -1 * std::numeric_limits<float>::max(); }));
		}
			SUBCASE("Handles Realistically sized problem") {
			const auto data = get_test_collection(30 * maximum_number_of_points);

			const auto[decimation_factor, decimated_data] = decimate_data(data);
				CHECK_EQ(decimation_factor, 30);
		}
	}
}
*/
