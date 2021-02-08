#include "AddStatePeriodRequest.h"
#include "../Conversions.h"


using namespace std::chrono;
using namespace std::chrono_literals;

state_period_t parse_new_period_from_request(const crow::request& request);
int get_state_id_at_time(const std::vector<state_change_t>& state_changes, const time_point_t& time);
std::vector<state_change_t> insert_new_state(std::vector<state_change_t>&& states, const state_period_t& new_state);
std::vector<state_change_t> remove_subsumed_states(std::vector<state_change_t>&& states, const state_period_t& new_state);

std::string AddStatePeriodRequest::add_state_period(const crow::request& request, std::shared_ptr<IConfigurationAccess>& config_storage, std::shared_ptr<IStateStorage>&
        state_storage) {
	const auto channel_id = std::stoi(request.url_params.get("channel_id"));
	const auto new_state_period = parse_new_period_from_request(request);

	const auto machine_id = config_storage->get_machine_id_from_channel_id(channel_id);

	auto existing_states = state_storage->get_state_changes_for_machine(machine_id);
	const auto new_states = insert_new_state(std::move(existing_states), new_state_period);

	//alarm_storage->update_state_changes_for_machine(machine_id); // TODO: This is absolute nonsense and we need some proper logic to fold in the news states in the alarm_storage.
	// ORM might have solved this...

	return ""; // TODO: Determine return for a failure case
}

state_period_t parse_new_period_from_request(const crow::request& request) {
	const auto start_time_seconds_since_epoch = std::stoi(request.url_params.get("start_seconds_since_epoch"));
	const auto end_time_seconds_since_epoch = std::stoi(request.url_params.get("end_seconds_since_epoch"));
	const auto state_id = std::stoi(request.url_params.get("state_id"));

	return state_period_t(state_id, time_point_t(seconds(start_time_seconds_since_epoch)), time_point_t(seconds(end_time_seconds_since_epoch)));
}

int get_state_id_at_time(const std::vector<state_change_t>& state_changes, const time_point_t& time) {
	const auto as_periods = Conversions::convert_state_changes_to_state_periods(state_changes);
	const auto state = std::lower_bound(as_periods.cbegin(), as_periods.cend(), time, [](const state_period_t& prd, const time_point_t& tp) { return prd.end < tp; });

	return state->state_id; // TODO: Handle exception case
}

std::vector<state_change_t> insert_new_state(std::vector<state_change_t>&& states, const state_period_t& new_state) {
	states = remove_subsumed_states(std::move(states), new_state);

	states.emplace_back(new_state.state_id, new_state.start);
	states.emplace_back(get_state_id_at_time(states, new_state.end), new_state.end);

	std::sort(states.begin(), states.end()); // Something about this transform can cause the incorrect numbers to end up in the final closing state.
	std::transform(states.cbegin() + 1, states.cend(), states.begin(), states.begin(), [](const auto& second, auto& first) {
		if (first.new_state_id == second.new_state_id)
			first.change_time = std::max(first.change_time, second.change_time);

		return first;
	});

	states.erase(std::unique(states.begin(), states.end()), states.end());

	return states;
}

std::vector<state_change_t> remove_subsumed_states(std::vector<state_change_t>&& states, const state_period_t& new_state) {
	auto as_periods = Conversions::convert_state_changes_to_state_periods(states); // Easier to work with them as periods rather than changes.

	const auto period_was_entirely_within_new_state = [&](const auto& state_period) { return state_period.start >= new_state.start && state_period.end <= new_state.end; };
	as_periods.erase(std::remove_if(as_periods.begin(), as_periods.end(), period_was_entirely_within_new_state), as_periods.end());

	std::vector<state_change_t> result;
	std::transform(as_periods.cbegin(), as_periods.cend(), std::back_inserter(result), [](const auto& period) { return state_change_t(period.state_id, period.start); });

	return result;
}