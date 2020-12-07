#include "AddStatePeriodRequest.h"

using namespace std::chrono;

state_period_t parse_new_period_from_request(const crow::request& request) {
	const auto start_time_seconds_since_epoch = std::stoi(request.url_params.get("start_seconds_since_epoch"));
	const auto end_time_seconds_since_epoch = std::stoi(request.url_params.get("end_seconds_since_epoch"));
	const auto state_id = std::stoi(request.url_params.get("state_id"));

	return state_period_t(state_id, time_point_t(seconds(start_time_seconds_since_epoch)), time_point_t(seconds(end_time_seconds_since_epoch)));
}

std::string AddStatePeriodRequest::add_state_period(const crow::request& request, std::shared_ptr<IDatabase>& database) {
	const auto channel_id = std::stoi(request.url_params.get("channel_id"));
	const auto new_state_period = parse_new_period_from_request(request);

	const auto existing_states = database->get_machine_id_from_channel_id(channel_id);
}


/*
 * This one is going to have most of the difficulty constrained to the testing of itself, because there are a lot of scenarios that need to be dealt with in an elegant fashion.
 *
 */
