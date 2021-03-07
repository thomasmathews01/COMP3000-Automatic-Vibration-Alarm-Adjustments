#include <vector>
#include "DatabaseInitialiser.h"
#include "database_statements.h"
#include "../Types/alarmTypes.h"
#include <array>
#include <range/v3/all.hpp>
#include <iostream>

std::shared_ptr<tao::pq::connection> DatabaseInitialiser::intialise_database(std::shared_ptr<tao::pq::connection>&& database) { // TODO: Make noexcept and encase in a try catch block
	const auto statements = {setup_alarm_settings, setup_alarm_levels, setup_alarm_activation_changes, setup_state_changes, setup_state_settings, create_types, create_sites, create_machines, create_channels, create_data};

	for (const auto& statement : statements)
		database->execute(statement);

	add_alarm_settings_entries_for_all_channels(database);

	return database;
}

void DatabaseInitialiser::add_alarm_settings_entries_for_all_channels(const std::shared_ptr<tao::pq::connection>& database) {
	const auto types_with_data_response = database->execute(get_all_types_with_data_associated_with_them);
	const auto types_with_data = types_with_data_response.vector<int>();

	const auto all_channels_response = database->execute(select_all_unique_channels);
	const auto all_channel_ids = all_channels_response.vector<int>();

	const auto all_alarm_severity_types = std::array<alarmSeverity, 2>{alarmSeverity::alert, alarmSeverity::alarm};

	for (const auto [channel, type, severity] : ranges::views::cartesian_product(all_channel_ids, types_with_data, all_alarm_severity_types)) {
		std::cout << "Adding settings for channel " << channel <<" type " << type << " severity " << static_cast<int>(severity) << std::endl;
		add_alarm_settings_if_not_exists(alarm_settings_t(channel, type, severity), database);
	}
}

void DatabaseInitialiser::add_alarm_settings_if_not_exists(const alarm_settings_t& settings, const std::shared_ptr<tao::pq::connection>& database) {
	const auto results = database->execute("SELECT COUNT(*) from alarm_settings where type_id = $1 and channel_id = $2 and alarm_severity = $3", settings.type_id, settings.channel_id, static_cast<int>(settings.severity));
	if (results.vector<int>().front() == 0)
		database->execute(insert_alarm_settings, settings.type_id, settings.channel_id, static_cast<int>(settings.severity), static_cast<int>(settings.threshold), settings.customLevel ? static_cast<int>(*settings.customLevel) : 0);
}
