#include "StateStorage.h"
#include <algorithm>
#include <range/v3/all.hpp>
#include "database_statements.h"

using namespace ranges;

namespace {
    int seconds_since_epoch(const time_point_t t) {
        return duration_cast<seconds>(t.time_since_epoch()).count();
    }

    std::string seconds_since_epoch_str(const time_point_t t) {
        return std::to_string(seconds_since_epoch(t));
    }
}

std::optional<time_point_t> StateStorage::get_time_of_last_state_change_before(const time_point_t& time, const int machine_id) {
    const auto statement = "SELECT max(time_since_epoch) from state_changes"
                           " WHERE machine_id = " + std::to_string(machine_id) +
                           " AND time_since_epoch < " + seconds_since_epoch_str(time);

    const auto result = database->execute(statement);
    const auto seconds_since_epoch = result.vector<int>().front();

	return time_point_t(seconds(seconds_since_epoch));
}

void StateStorage::add_new_state_period(int machine_id, state_period_t state_period) {
    const auto state_when_period_ends = get_state_at_time(state_period.end, machine_id);

    if (!state_when_period_ends.has_value())
        return;

    database->execute("DELETE FROM state_changes where time_since_epoch BETWEEN $1 and $2",
					  seconds_since_epoch(state_period.start),
					  seconds_since_epoch(state_period.end));

    add_new_state_changes_for_machine({state_change_t(state_period.state_id, state_period.start),
                                       state_change_t(*state_when_period_ends, state_period.end)}, machine_id);
}

std::vector<state_change_t> StateStorage::get_state_changes_for_machine(int machine_id) {
	const auto statement =
		"SELECT new_state_id,time_since_epoch FROM state_changes WHERE machine_id = " + std::to_string(machine_id);

	const auto result = database->execute(statement);
	const auto result_values = result.vector<std::tuple<int, int>>();

	std::vector<state_change_t> state_changes;
	state_changes.reserve(result_values.size());

	for (const auto[state, time_seconds] : result_values)
		state_changes.emplace_back(state, time_point_t(seconds(time_seconds)));

	return state_changes;
}

void StateStorage::add_state(int machine_id, const state_t& state) {
	//TODO: Implement
}

void StateStorage::remove_state(int machine_id, int state_id) {
	//TODO: Implement
}

std::vector<state_t> StateStorage::get_states_for_machine(int machine_id) {
	//TODO: Implement
    return std::vector<state_t>();
}

std::optional<int> StateStorage::get_state_at_time(const time_point_t& time, const int machine_id) {
    const auto time_of_final_state_change_before_time = get_time_of_last_state_change_before(time, machine_id);

    if (!time_of_final_state_change_before_time.has_value())
        return std::nullopt;

    const auto final_state_time_seconds = seconds_since_epoch(*time_of_final_state_change_before_time);
    const auto statement = "SELECT new_state_id from state_changes where time_since_epoch = " +
                           std::to_string(final_state_time_seconds);

    const auto select_result = database->execute(statement);
	return select_result.vector<int>().front();
}

void StateStorage::add_new_state_changes_for_machine(const std::vector<state_change_t>& changes, const int machine_id) {
    for (const auto& change : changes)
		database->execute(add_new_state_change, machine_id, change.new_state_id, seconds_since_epoch(change.change_time));
}
