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
}

std::vector<alarm_settings_t> AlarmStorage::get_alarm_settings_for_machine(int machine_id) {
    
    const auto statement =
            "SELECT alarm_settings.channel_id, alarm_settings.type_id, alarm_severity, custom_fixed_threshold, alarm_threshold_type "
            "FROM alarm_settings inner join channels on channels.channel_id = alarm_settings.channel_id "
            "WHERE channels.machine_id = " + std::to_string(machine_id);

    const auto query_response = database->execute(statement);
    const auto result_values = query_response.vector<std::tuple<int, int, int, float, int>>();

    std::vector<alarm_settings_t> alarm_settings;
    alarm_settings.reserve(result_values.size());

    for (const auto[channel, type, severity, threshold, threshold_type] : result_values) {
		const auto threshold_enum = static_cast<alarmThreshold>(threshold_type);
		const auto severity_enum = static_cast<alarmSeverity>(severity);
		const auto threshold_value = threshold_enum == alarmThreshold::Custom ? std::make_optional(threshold) : std::nullopt;

		alarm_settings.emplace_back(channel, type, severity_enum, threshold_enum, threshold_value);
    }

	return alarm_settings;
}

bool AlarmStorage::update_alarm_setting(const alarm_settings_t& new_setting) {
	const auto result = database->execute(modify_alarm_settings, static_cast<int>(new_setting.threshold), static_cast<int>(*new_setting.customLevel), new_setting.channel_id, new_setting.type_id, static_cast<int>(new_setting.severity));

	return result.rows_affected() > 0;
}

std::vector<alarm_level_history_point> AlarmStorage::get_alarm_level_history(const alarm_settings_t& associated_alarm) {
    const auto statement = "SELECT level, time_since_epoch FROM alarm_levels WHERE channel_id = " +
                           std::to_string(associated_alarm.channel_id) +
                           " AND type_id = " + std::to_string(associated_alarm.type_id) + " AND alarm_severity = " +
                           std::to_string(static_cast<int>(associated_alarm.severity));
    const auto results = database->execute(statement);
    const auto result_values = results.vector<std::tuple<double, int>>();

    std::vector<alarm_level_history_point> history;

    for (const auto[level, seconds_since_epoch] : result_values)
    	history.emplace_back(time_point_t(seconds(seconds_since_epoch)), level);

	return history;
}

void AlarmStorage::add_alarm_level_history_item(const time_point_t& occurence, const alarm_settings_t& associated_alarm,
                                            double new_level) {
	database->execute(add_alarm_level_history_point, associated_alarm.channel_id, associated_alarm.type_id,
									   static_cast<int>(associated_alarm.severity), new_level, seconds_since_epoch(occurence));
}

void AlarmStorage::add_alarm_activation(const alarm_activation_t& activation) {
	database->execute(add_alarm_activation_change, activation.channel_id, activation.type_id, static_cast<int>(activation.severity), activation.severity != alarmSeverity::none, seconds_since_epoch(activation.activation_time));
}

std::vector<alarm_activation_t> AlarmStorage::get_activations_for_machine(int machine_id) {
    const auto statement =
            "SELECT alarm_activation_changes.type_id, alarm_activation_changes.channel_id, alarm_activation_changes.alarm_severity, alarm_activation_changes.became_active, alarm_activation_changes.time_since_epoch FROM alarm_activation_changes "
            "INNER JOIN channels on channels.channel_id = alarm_activation_changes.channel_id "
            "WHERE channels.machine_id = " + std::to_string(machine_id) +
            " ORDER BY alarm_activation_changes.time_since_epoch";

    std::vector<alarm_activation_t> alarm_activations;

    const auto results = database->execute(statement);
    const auto result_values = results.vector<std::tuple<int, int, int, bool, int>>();
    alarm_activations.reserve(result_values.size());

    for (const auto[type, channel, severity, became_active, time_since_epoch] : result_values)
    	alarm_activations.emplace_back(channel, type, static_cast<alarmSeverity>(severity), time_point_t(seconds(time_since_epoch)), became_active);


	return alarm_activations;
}

alarm_state_t AlarmStorage::get_alarm_state_of_site(const site& site) {
    return alarm_state_t(std::nullopt); // TODO: Implement
}

alarm_state_t AlarmStorage::get_alarm_state_of_machine(const machine& machine) {
    return alarm_state_t(std::nullopt); // TODO: Implement
}

alarm_state_t AlarmStorage::get_alarm_state_of_channel(const channel& channel) {
    return alarm_state_t(std::nullopt); // TODO: Implement
}

alarm_state_t AlarmStorage::get_alarm_state_of_alarm(const alarm_settings_t& alarm) {
    return alarm_state_t(std::nullopt); // TODO: Implement
}

std::vector<alarm_settings_t> AlarmStorage::get_all_alarm_settings() {
    const auto statement = "SELECT alarm_settings.channel_id, alarm_settings.type_id, alarm_severity, custom_fixed_threshold, alarm_threshold_type "
                           "FROM alarm_settings";

    const auto result = database->execute(statement);
    const auto result_values = result.vector<std::tuple<int, int, int, float, int>>();

    std::vector<alarm_settings_t> settings;

    for(const auto[channel, type, severity, threshold, threshold_type] : result_values) {
		const auto threshold_enum = static_cast<alarmThreshold>(threshold_type);
		const auto severity_enum = static_cast<alarmSeverity>(severity);
		const auto threshold_value =
			threshold_enum == alarmThreshold::Custom ? std::make_optional(threshold) : std::nullopt;

		settings.emplace_back(channel, type, severity_enum, threshold_enum, threshold_value);
    }

	return settings;
}

alarm_settings_t AlarmStorage::get_updated_alarm_settings(const alarm_settings_t& previous_settings) {
    const auto statement = "SELECT custom_fixed_threshold, alarm_threshold_type"
                           " FROM alarm_settings"
                           " WHERE channel_id = " + std::to_string(previous_settings.channel_id) +
                           " AND type_id = " + std::to_string(previous_settings.type_id) +
                           " AND alarm_severity = " + std::to_string(static_cast<int>(previous_settings.severity));

    const auto results = database->execute(statement);
    const auto result_values = results.vector<std::tuple<float, int>>();

	if (!result_values.empty())
	{
		const auto[threshold, threshold_type] = result_values.front();

		const auto threshold_type_enum = static_cast<alarmThreshold>(threshold_type);

		return alarm_settings_t(previous_settings.channel_id, previous_settings.type_id, previous_settings.severity,
								threshold_type_enum,
								threshold_type_enum == alarmThreshold::Custom ? std::make_optional(threshold)
																			  : std::nullopt);
	}

	throw std::runtime_error("Something has gone terribly wrong.");
}
