#include "Database.h"
#include "DatabaseInitialiser.h"
#include "database_statements.h"
#include <range/v3/all.hpp>

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace std::string_literals;
using namespace ranges;

namespace {
    int seconds_since_epoch(const time_point_t t) {
        return duration_cast<seconds>(t.time_since_epoch()).count();
    }

    std::string seconds_since_epoch_str(const time_point_t t) {
        return std::to_string(seconds_since_epoch(t));
    }

    template<class T, class F>
    std::vector<T>
    get_query_results(const char *statement, const std::shared_ptr<sqlite3pp::database>& database, F&& func) {
        std::vector<T> result;

        auto query_results = sqlite3pp::query(*database, statement);
        std::transform(query_results.begin(), query_results.end(), std::back_inserter(result), func);

        return result;
    }
}

std::vector<site> Database::get_site_data() {
    std::lock_guard guard(database_access_mutex);

    return get_query_results<site>("SELECT site_id, site_name from SITES", database, [](const auto& row) {
        const auto[id_number, name_string] = row.template get_columns<int, const char *>(0, 1);
        return site(id_number, name_string);
    });
}

std::vector<machine> Database::get_machines_for_site(const site& site) {
    std::lock_guard guard(database_access_mutex);

    return get_query_results<machine>(find_machine_for_site(site.id).c_str(), database, [](const auto& row) {
        const auto[id, name] = row.template get_columns<int, const char *>(0, 1);

        return machine(id, name);
    });
}

std::vector<channel> Database::get_channel_information_for_machine(const machine& machine) {
    std::lock_guard guard(database_access_mutex);

    return get_query_results<channel>(find_channels_for_machine(machine.id).c_str(), database, [](const auto& row) {
        const auto[id, name, units] = row.template get_columns<int, const char *, const char *>(0, 1, 2);

        return channel(id, name, units);
    });
}

std::vector<std::pair<time_point_t, float>>
Database::get_data(int channel, int type, time_point_t start, time_point_t finish) {
    std::lock_guard guard(database_access_mutex);

    const auto statement = "SELECT time_since_epoch, value FROM data"s +
                           " WHERE type_id =  " + std::to_string(type) +
                           " AND time_since_epoch BETWEEN " + seconds_since_epoch_str(start) + " AND " +
                           std::to_string(seconds_since_epoch(finish)) +
                           " AND channel_id = " + std::to_string(channel) + " ";

    return get_query_results<std::pair<time_point_t, float>>(statement.c_str(), database, [](const auto& row) {
        const auto[epoch_time, value] = row.template get_columns<int, float>(0, 1);

        return std::make_pair(time_point_t(seconds(epoch_time)), value);
    });
}

std::vector<std::pair<int, std::string>> Database::get_data_types_available_for_channel(int channel_id) {
    const auto statement =
            "SELECT DISTINCT types.type_id, types.type_name FROM data INNER JOIN types on types.type_id = data.type_id WHERE channel_id = " +
            std::to_string(channel_id);

    return get_query_results<std::pair<int, std::string>>(statement.c_str(), database, [](const auto& row) {
        const auto[type_id, type_name] = row.template get_columns<int, const char *>(0, 1);
        return std::make_pair(type_id, type_name);
    });
}

int Database::get_machine_id_from_channel_id(int channel_id) {
    std::lock_guard guard(database_access_mutex);

    const auto selection_string = "SELECT machine_id from channels where channel_id = " + std::to_string(channel_id);

    for (const auto& row : sqlite3pp::query(*database, selection_string.c_str())) {
        const auto[machine_id] = row.template get_columns<int>(0);
        return machine_id;
    }

    return -1;
}

time_point_t Database::get_earliest_data_point_for_machine(int machine_id) {
    const auto selection_string =
            "SELECT min(data.time_since_epoch) from data inner join channels on data.channel_id = channels.channel_id "
            "inner join machines on channels.machine_id = machines.machine_id where machines.machine_id = " +
            std::to_string(machine_id);
    for (const auto& row : sqlite3pp::query(*database, selection_string.c_str())) {
        const auto[seconds_since_epoch] = row.template get_columns<int>(0);
        return time_point_t(seconds(seconds_since_epoch));
    }

    return time_point_t(0s);
}

std::optional<time_point_t>
Database::get_time_of_last_state_change_before(const time_point_t& time, const int machine_id) {
    const auto statement = "SELECT max(time_since_epoch) from state_changes"
                           " WHERE machine_id = " + std::to_string(machine_id) +
                           " AND time_since_epoch < " + seconds_since_epoch_str(time);

    for (const auto& row : sqlite3pp::query(*database, statement.c_str())) {
        const auto[seconds_since_epoch] = row.template get_columns<int>(0);
        return time_point_t(seconds(seconds_since_epoch));
    }

    return std::nullopt;
}

std::optional<int> Database::get_state_at_time(const time_point_t& time, const int machine_id) {
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

void Database::add_new_state_changes_for_machine(const std::vector<state_change_t>& changes, const int machine_id) {
    for (const auto& change : changes) {
        sqlite3pp::command cmd(*database, add_new_state_change);

        cmd.bind(":machine", machine_id);
        cmd.bind(":state", change.new_state_id);
        cmd.bind(":time", seconds_since_epoch(change.change_time));

        cmd.execute();
    }
}

void Database::add_new_state_period(int machine_id, state_period_t state_period) {
    std::lock_guard guard(database_access_mutex);

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

std::vector<state_change_t> Database::get_state_changes_for_machine(int machine_id) {
    const auto statement =
            "SELECT new_state_id,time_since_epoch FROM state_changes WHERE machine_id = " + std::to_string(machine_id);

    return get_query_results<state_change_t>(statement.c_str(), database, [](const auto& row) {
        const auto[state, time_seconds] = row.template get_columns<int, int>(0, 1);

        return state_change_t(state, time_point_t(seconds(time_seconds)));
    });
}


std::vector<alarm_settings_t> Database::get_alarm_settings_for_machine(int machine_id) {
    std::lock_guard guard(database_access_mutex);
    const auto statement =
            "SELECT alarm_settings.channel_id, alarm_settings.type_id, alarm_severity, custom_fixed_threshold, alarm_threshold_type "
            "FROM alarm_settings inner join channels on channels.channel_id = alarm_settings.channel_id "
            "WHERE channels.machine_id = " + std::to_string(machine_id);

    return get_query_results<alarm_settings_t>(statement.c_str(), database, [](const auto& row) {
        const auto[channel, type, severity, threshold, threshold_type] = row.template get_columns<int, int, int, float, int>(
                0, 1, 2, 3, 4);
        const auto threshold_enum = static_cast<alarmThreshold>(threshold_type);
        const auto severity_enum = static_cast<alarmSeverity>(severity);
        const auto threshold_value =
                threshold_enum == alarmThreshold::Custom ? std::make_optional(threshold) : std::nullopt;

        return alarm_settings_t(channel, type, severity_enum, threshold_enum, threshold_value);
    });
}

bool Database::update_alarm_setting(const alarm_settings_t& new_setting) {
    std::lock_guard guard(database_access_mutex);

    sqlite3pp::command cmd(*database, modify_alarm_settings);

    cmd.bind(":type", new_setting.type_id);
    cmd.bind(":channel", new_setting.channel_id);
    cmd.bind(":severity", static_cast<int>(new_setting.severity));
    cmd.bind(":threshold_type", static_cast<int>(new_setting.threshold));
    cmd.bind(":fixed_threshold", new_setting.customLevel ? static_cast<int>(*new_setting.customLevel) : 0);

    return cmd.execute();
}

std::vector<alarm_level_history_point> Database::get_alarm_level_history(const alarm_settings_t& associated_alarm) {
    std::lock_guard guard(database_access_mutex);

    const auto statement = "SELECT level, time_since_epoch FROM alarm_levels WHERE channel_id = " +
                           std::to_string(associated_alarm.channel_id) +
                           " AND type_id = " + std::to_string(associated_alarm.type_id) + " AND alarm_severity = " +
                           std::to_string(static_cast<int>(associated_alarm.severity));

    return get_query_results<alarm_level_history_point>(statement.c_str(), database, [](const auto& row) {
        const auto[level, seconds_since_epoch] = row.template get_columns<double, int>(0, 1);

        return alarm_level_history_point(time_point_t(seconds(seconds_since_epoch)), level);
    });
}

void Database::add_alarm_level_history_item(const time_point_t& occurence, const alarm_settings_t& associated_alarm,
                                            double new_level) {
    std::lock_guard guard(database_access_mutex);

    sqlite3pp::command cmd(*database, add_alarm_level_history_point);

    cmd.bind(":type", associated_alarm.type_id);
    cmd.bind(":channel", associated_alarm.channel_id);
    cmd.bind(":severity", static_cast<int>(associated_alarm.severity));
    cmd.bind(":level", new_level);
    cmd.bind(":time_since_epoch", seconds_since_epoch(occurence));

    cmd.execute();
}

Database::Database(std::shared_ptr<IDatabaseFactory> db_factory) : factory(std::move(db_factory)) {
    database = DatabaseInitialiser::intialise_database(factory->get_database());
}

void Database::add_alarm_activation(const alarm_activation_t& activation) {
    std::lock_guard guard(database_access_mutex);

    sqlite3pp::command cmd(*database, add_alarm_activation_change);

    cmd.bind(":type", activation.type_id);
    cmd.bind(":channel", activation.channel_id);
    cmd.bind(":severity", static_cast<int>(activation.severity));
    cmd.bind(":became_active", activation.severity != alarmSeverity::none);
    cmd.bind(":time_since_epoch", seconds_since_epoch(activation.activation_time));

    cmd.execute();
}

std::vector<alarm_activation_t> Database::get_activations_for_machine(int machine_id) {
    std::lock_guard guard(database_access_mutex);

    const auto statement =
            "SELECT alarm_activation_changes.type_id, alarm_activation_changes.channel_id, alarm_activation_changes.alarm_severity, alarm_activation_changes.became_active, alarm_activation_changes.time_since_epoch FROM alarm_activation_changes "
            "INNER JOIN channels on channels.channel_id = alarm_activation_changes.channel_id "
            "WHERE channels.machine_id = " + std::to_string(machine_id) +
            " ORDER BY alarm_activation_changes.time_since_epoch";

    return get_query_results<alarm_activation_t>(statement.c_str(), database, [](const auto& row) {
        const auto[type, channel, severity, became_active, time_since_epoch] = row.template get_columns<int, int, int, bool, int>(
                0, 1, 2, 3, 4);

        return alarm_activation_t(channel, type, static_cast<alarmSeverity>(severity),
                                  time_point_t(seconds(time_since_epoch)), became_active);
    });
}

alarm_state_t Database::get_alarm_state_of_site(const site& site) {
    return alarm_state_t(std::nullopt);
}

alarm_state_t Database::get_alarm_state_of_machine(const machine& machine) {
    return alarm_state_t(std::nullopt);
}

alarm_state_t Database::get_alarm_state_of_channel(const channel& channel) {
    return alarm_state_t(std::nullopt);
}

void Database::add_state(int machine_id, const state_t& state) {

}

void Database::remove_state(int machine_id, int state_id) {

}

std::vector<state_t> Database::get_states_for_machine(int machine_id) {
    return std::vector<state_t>();
}

alarm_state_t Database::get_alarm_state_of_alarm(const alarm_settings_t& alarm) {
    return alarm_state_t(std::nullopt);
}

std::vector<alarm_settings_t> Database::get_all_alarm_settings() {
    std::lock_guard guard(database_access_mutex);
    const auto statement = "SELECT alarm_settings.channel_id, alarm_settings.type_id, alarm_severity, custom_fixed_threshold, alarm_threshold_type "
                           "FROM alarm_settings";

    return get_query_results<alarm_settings_t>(statement, database, [](const auto& row) {
        const auto[channel, type, severity, threshold, threshold_type] = row.template get_columns<int, int, int, float, int>(
                0, 1, 2, 3, 4);
        const auto threshold_enum = static_cast<alarmThreshold>(threshold_type);
        const auto severity_enum = static_cast<alarmSeverity>(severity);
        const auto threshold_value =
                threshold_enum == alarmThreshold::Custom ? std::make_optional(threshold) : std::nullopt;

        return alarm_settings_t(channel, type, severity_enum, threshold_enum, threshold_value);
    });
}

alarm_settings_t Database::get_updated_alarm_settings(const alarm_settings_t& previous_settings) {
    std::lock_guard guard(database_access_mutex);
    const auto statement = "SELECT custom_fixed_threshold, alarm_threshold_type"
                           " FROM alarm_settings"
                           " WHERE channel_id = " + std::to_string(previous_settings.channel_id) +
                           " AND type_id = " + std::to_string(previous_settings.type_id) +
                           " AND alarm_severity = " + std::to_string(static_cast<int>(previous_settings.severity));

    auto query_results = sqlite3pp::query(*database, statement.c_str());
    // TODO: What if we don't find any results? Should probably check and at least return an optional. We don't want async exception throwing, this code needs to be fast and non blocking as much as possible.

    const auto first_row = query_results.begin().operator*();
    const auto[threshold, threshold_type] = first_row.get_columns<float, int>(0, 1);
    const auto threshold_type_enum = static_cast<alarmThreshold>(threshold_type);

    return alarm_settings_t(previous_settings.channel_id, previous_settings.type_id, previous_settings.severity,
                            threshold_type_enum,
                            threshold_type_enum == alarmThreshold::Custom ? std::make_optional(threshold)
                                                                          : std::nullopt);
}

std::pair<time_point_t, float> Database::get_last_data_point_before(int channel, int type, time_point_t time) {
    std::lock_guard guard(database_access_mutex);

    const auto statement = "SELECT time_since_epoch, value FROM data"s +
                           " WHERE type_id =  " + std::to_string(type) +
                           " AND time_since_epoch <= " + seconds_since_epoch_str(time) +
                           " AND channel_id = " + std::to_string(channel) +
                           " ORDER BY time_since_epoch DESC" +
                           " LIMIT 1";

    auto query_results = sqlite3pp::query(*database, statement.c_str());
    // TODO: What if we don't find any results? Should probably check and at least return an optional. We don't want async exception throwing, this code needs to be fast and non blocking as much as possible.

    const auto first_row = query_results.begin().operator*();
    const auto[epoch_time, value] = first_row.get_columns<int, float>(0, 1);

	return std::make_pair(time_point_t(seconds(epoch_time)), value);
}

statistics_point_t Database::get_last_statistics_calculation(int channel, int type) {
    std::lock_guard guard(database_access_mutex);
    const auto map_index = std::make_pair(channel, type);
    const auto matching_statistic = latest_statistics.emplace(map_index, statistics_point_t());

    return matching_statistic.first->second;
}

void Database::update_last_statistics_calculation(int channel, int type, const statistics_point_t& new_values) {
    std::lock_guard guard(database_access_mutex);
    latest_statistics.at({channel, type}) = new_values;
}

std::vector<float> Database::get_data_points_only(int channel, int type, time_point_t start, time_point_t finish) {
    std::lock_guard guard(database_access_mutex);

    const auto statement = "SELECT value FROM data"s +
                           " WHERE type_id =  " + std::to_string(type) +
                           " AND channel_id = " + std::to_string(channel) +
                           " AND time_since_epoch BETWEEN " + seconds_since_epoch_str(start + 1s) + " AND " +
                           std::to_string(seconds_since_epoch(finish));

    return get_query_results<float>(statement.c_str(), database, [](const auto& row) {
        const auto[value] = row.template get_columns<float>(0);
        return value;
    });
}

std::vector<channel> Database::get_all_channels() {
    std::lock_guard guard(database_access_mutex);

    std::vector<channel> channels;

    for (const auto& site : get_site_data())
        for (const auto& machine : get_machines_for_site(site))
            channels |= actions::push_back(get_channel_information_for_machine(machine));

    return channels;
}

std::vector<int> Database::get_all_data_types() {
    const auto statement = "SELECT DISTINCT type_id FROM types";

    return get_query_results<int>(statement, database, [](const auto& row) {
        const auto[type_id] = row.template get_columns<int>(0);
        return type_id;
    });
}
