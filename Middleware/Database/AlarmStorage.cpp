#include "AlarmStorage.h"
#include "database_statements.h"
#include <algorithm>

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

std::vector<alarm_settings_t> AlarmStorage::get_alarm_settings_for_machine(int machine_id) {
    
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

bool AlarmStorage::update_alarm_setting(const alarm_settings_t& new_setting) {
    

    sqlite3pp::command cmd(*database, modify_alarm_settings);

    cmd.bind(":type", new_setting.type_id);
    cmd.bind(":channel", new_setting.channel_id);
    cmd.bind(":severity", static_cast<int>(new_setting.severity));
    cmd.bind(":threshold_type", static_cast<int>(new_setting.threshold));
    cmd.bind(":fixed_threshold", new_setting.customLevel ? static_cast<int>(*new_setting.customLevel) : 0);

    return cmd.execute();
}

std::vector<alarm_level_history_point> AlarmStorage::get_alarm_level_history(const alarm_settings_t& associated_alarm) {
    

    const auto statement = "SELECT level, time_since_epoch FROM alarm_levels WHERE channel_id = " +
                           std::to_string(associated_alarm.channel_id) +
                           " AND type_id = " + std::to_string(associated_alarm.type_id) + " AND alarm_severity = " +
                           std::to_string(static_cast<int>(associated_alarm.severity));

    return get_query_results<alarm_level_history_point>(statement.c_str(), database, [](const auto& row) {
        const auto[level, seconds_since_epoch] = row.template get_columns<double, int>(0, 1);

        return alarm_level_history_point(time_point_t(seconds(seconds_since_epoch)), level);
    });
}

void AlarmStorage::add_alarm_level_history_item(const time_point_t& occurence, const alarm_settings_t& associated_alarm,
                                            double new_level) {
    

    sqlite3pp::command cmd(*database, add_alarm_level_history_point);

    cmd.bind(":type", associated_alarm.type_id);
    cmd.bind(":channel", associated_alarm.channel_id);
    cmd.bind(":severity", static_cast<int>(associated_alarm.severity));
    cmd.bind(":level", new_level);
    cmd.bind(":time_since_epoch", seconds_since_epoch(occurence));

    cmd.execute();
}

void AlarmStorage::add_alarm_activation(const alarm_activation_t& activation) {
    sqlite3pp::command cmd(*database, add_alarm_activation_change);

    cmd.bind(":type", activation.type_id);
    cmd.bind(":channel", activation.channel_id);
    cmd.bind(":severity", static_cast<int>(activation.severity));
    cmd.bind(":became_active", activation.severity != alarmSeverity::none);
    cmd.bind(":time_since_epoch", seconds_since_epoch(activation.activation_time));

    cmd.execute();
}

std::vector<alarm_activation_t> AlarmStorage::get_activations_for_machine(int machine_id) {
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

alarm_state_t AlarmStorage::get_alarm_state_of_site(const site& site) {
    return alarm_state_t(std::nullopt);
}

alarm_state_t AlarmStorage::get_alarm_state_of_machine(const machine& machine) {
    return alarm_state_t(std::nullopt);
}

alarm_state_t AlarmStorage::get_alarm_state_of_channel(const channel& channel) {
    return alarm_state_t(std::nullopt);
}

alarm_state_t AlarmStorage::get_alarm_state_of_alarm(const alarm_settings_t& alarm) {
    return alarm_state_t(std::nullopt);
}

std::vector<alarm_settings_t> AlarmStorage::get_all_alarm_settings() {
    
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

alarm_settings_t AlarmStorage::get_updated_alarm_settings(const alarm_settings_t& previous_settings) {
    
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
