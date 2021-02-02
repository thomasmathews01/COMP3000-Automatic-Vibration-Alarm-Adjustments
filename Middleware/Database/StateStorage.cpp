#include "StateStorage.h"
#include <algorithm>
#include "database_statements.h"

namespace {
    int seconds_since_epoch(const time_point_t t) {
        return duration_cast<seconds>(t.time_since_epoch()).count();
    }

    std::string seconds_since_epoch_str(const time_point_t t) {
        return std::to_string(seconds_since_epoch(t));
    }

    template<class T, class F>
    std::vector<T> get_query_results(const char *statement, const std::shared_ptr<sqlite3pp::database>& database, F&& func) {
        std::vector<T> result;

        auto query_results = sqlite3pp::query(*database, statement);
        std::transform(query_results.begin(), query_results.end(), std::back_inserter(result), func);

        return result;
    }
}

std::optional<time_point_t> StateStorage::get_time_of_last_state_change_before(const time_point_t& time, const int machine_id) {
    const auto statement = "SELECT max(time_since_epoch) from state_changes"
                           " WHERE machine_id = " + std::to_string(machine_id) +
                           " AND time_since_epoch < " + seconds_since_epoch_str(time);

    for (const auto& row : sqlite3pp::query(*database, statement.c_str())) {
        const auto[seconds_since_epoch] = row.template get_columns<int>(0);
        return time_point_t(seconds(seconds_since_epoch));
    }

    return std::nullopt;
}

void StateStorage::add_new_state_period(int machine_id, state_period_t state_period) {
    const auto state_when_period_ends = get_state_at_time(state_period.end, machine_id);

    if (!state_when_period_ends.has_value())
        return;

    sqlite3pp::command cmd(*database, "DELETE FROM state_changes where time_since_epoch BETWEEN :start AND :end");

    cmd.bind(":start", seconds_since_epoch(state_period.start));
    cmd.bind(":end", seconds_since_epoch(state_period.end));

    cmd.execute();

    add_new_state_changes_for_machine({state_change_t(state_period.state_id, state_period.start),
                                       state_change_t(*state_when_period_ends, state_period.end)}, machine_id);
}

std::vector<state_change_t> StateStorage::get_state_changes_for_machine(int machine_id) {
    const auto statement =
            "SELECT new_state_id,time_since_epoch FROM state_changes WHERE machine_id = " + std::to_string(machine_id);

    return get_query_results<state_change_t>(statement.c_str(), database, [](const auto& row) {
        const auto[state, time_seconds] = row.template get_columns<int, int>(0, 1);

        return state_change_t(state, time_point_t(seconds(time_seconds)));
    });
}
void StateStorage::add_state(int machine_id, const state_t& state) {

}

void StateStorage::remove_state(int machine_id, int state_id) {

}

std::vector<state_t> StateStorage::get_states_for_machine(int machine_id) {
    return std::vector<state_t>();
}

std::optional<int> StateStorage::get_state_at_time(const time_point_t& time, const int machine_id) {
    const auto time_of_final_state_change_before_time = get_time_of_last_state_change_before(time, machine_id);

    if (!time_of_final_state_change_before_time.has_value())
        return std::nullopt;

    const auto final_state_time_seconds = seconds_since_epoch(*time_of_final_state_change_before_time);
    const auto statement = "SELECT new_state_id from state_changes where time_since_epoch = " +
                           std::to_string(final_state_time_seconds);

    for (const auto& row : sqlite3pp::query(*database, statement.c_str())) {
        const auto[state_id] = row.template get_columns<int>(0);
        return state_id;
    }

    return std::nullopt;
}

void StateStorage::add_new_state_changes_for_machine(const std::vector<state_change_t>& changes, const int machine_id) {
    for (const auto& change : changes) {
        sqlite3pp::command cmd(*database, add_new_state_change);

        cmd.bind(":machine", machine_id);
        cmd.bind(":state", change.new_state_id);
        cmd.bind(":time", seconds_since_epoch(change.change_time));

        cmd.execute();
    }
}
