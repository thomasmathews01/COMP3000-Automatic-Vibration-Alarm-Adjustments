#include <vector>
#include "DatabaseInitialiser.h"
#include "database_statements.h"
#include "../Types/alarmTypes.h"

void DatabaseInitialiser::intialise_database(std::shared_ptr<sqlite3pp::database> database) { // TODO: Make noexcept and encase in a try catch block
	const auto statements = {setup_alarm_settings, setup_alarm_levels, setup_alarm_activation_changes, setup_state_changes, setup_state_settings, create_types, create_sites, create_machines, create_channels, create_data};

	for (const auto& statement : statements)
		database->execute(statement); // TODO: We should check the return value, to ensure all the tables were actually created

	add_alarm_settings_entries_for_all_channels(database);
}

void DatabaseInitialiser::add_alarm_settings_entries_for_all_channels(std::shared_ptr<sqlite3pp::database> database) {
	// TODO: Should this access also be mutex protected if we have a more complex initialisation sequence?

	std::vector<int> types_with_data = {};

	for (const auto& row : sqlite3pp::query(*database, get_all_types_with_data_associated_with_them)) {
		const auto[type_id] = row.get_columns<int>(0);
		types_with_data.emplace_back(type_id);
	}

	std::vector<alarm_settings_t> alarm_settings;

	for (const auto& row : sqlite3pp::query(*database, select_all_unique_channels)) {
		const auto[channel] = row.get_columns<int>(0);

		for (const auto severity : {alarmSeverity::alert, alarmSeverity::alarm})
			for (const auto type : types_with_data)
				alarm_settings.emplace_back(channel, type, severity);
	}

	sqlite3pp::transaction xct(*database);
	{
		for (const auto settings : alarm_settings)
			add_alarm_settings_if_not_exists(settings, database);
	}
}

void DatabaseInitialiser::add_alarm_settings_if_not_exists(const alarm_settings_t settings, std::shared_ptr<sqlite3pp::database> database) {
	sqlite3pp::command cmd(*database, add_alarm_settings);

	cmd.bind(":type", std::to_string(settings.type_id), sqlite3pp::nocopy);
	cmd.bind(":channel", std::to_string(settings.channel_id), sqlite3pp::nocopy);
	cmd.bind(":severity", std::to_string(static_cast<int>(settings.severity)), sqlite3pp::nocopy);
	cmd.bind(":threshold_type", std::to_string(static_cast<int>(settings.threshold)), sqlite3pp::nocopy);
	cmd.bind(":custom_threshold", std::to_string(settings.customLevel ? static_cast<int>(*settings.customLevel) : 0), sqlite3pp::nocopy);

	cmd.execute();
}
