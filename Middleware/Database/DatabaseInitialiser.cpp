#include "DatabaseInitialiser.h"

constexpr auto setup_alarm_settings = "CREATE TABLE IF NOT EXISTS alarm_settings (\n"
									  "\talarm_settings_id INTEGER PRIMARY KEY,\n"
									  "\tchannel_id INTEGER,\n"
									  "\ttype_id INTEGER, \n"
									  "\talarm_severity INTEGER,\n"
									  "\talarm_threshold_type INTEGER,\n"
									  "\tcustom_fixed_threshold REAL,\n"
									  "\tFOREIGN KEY(channel_id) REFERENCES channels(channel_id),\n"
									  "\tFOREIGN KEY(type_id) REFERENCES types(type_id)\n"
									  ");";

constexpr auto setup_alarm_levels = "CREATE TABLE IF NOT EXISTS alarm_levels (\n"
									"\talarm_settings_id INTEGER,\n"
									"\tlevel REAL,\n"
									"\ttime_since_epoch INTEGER,\n"
									"\tPRIMARY KEY(alarm_settings_id, level, time_since_epoch),\n"
									"\tFOREIGN KEY (alarm_settings_id) REFERENCES alarm_settings(alarm_settings_id)\n"
									");";

constexpr auto setup_alarm_activation_changes = "CREATE TABLE IF NOT EXISTS alarm_activation_changes (\n"
												"\talarm_settings_id INTEGER,\n"
												"\tbecame_active BOOLEAN ,\n"
												"\ttime_since_epoch INTEGER,\n"
												"\tPRIMARY KEY(alarm_settings_id, became_active, time_since_epoch),\n"
												"\tFOREIGN KEY (alarm_settings_id) REFERENCES alarm_settings(alarm_settings_id)\n"
												");";

constexpr auto setup_state_settings = "CREATE TABLE IF NOT EXISTS state_settings (\n"
									  "\tstate_id INTEGER PRIMARY KEY,\n"
									  "\tmachine_id INTEGER,\n"
									  "\tstate_name TEXT,\n"
									  "\tFOREIGN KEY(machine_id) REFERENCES machines(machine_id)\n"
									  ");";

constexpr auto setup_state_changes = "CREATE TABLE IF NOT EXISTS state_changes (\n"
									 "\tmachine_id INTEGER,\n"
									 "\tnew_state_id INTEGER,\n"
									 "\ttime_since_epoch INTEGER,\n"
									 "\tFOREIGN KEY (new_state_id) REFERENCES state_settings(state_id),\n"
									 "\tFOREIGN KEY (machine_id) REFERENCES machines(machine_id)\n"
									 "\n"
									 ");";

void DatabaseInitialiser::intialise_database(sqlite3pp::database& database) { // TODO: Make noexcept and encase in a try catch block
	const auto statements = {setup_alarm_settings, setup_alarm_levels, setup_alarm_activation_changes, setup_state_changes, setup_state_settings};

	for (const auto& statement : statements)
		database.execute(statement); // TODO: We should check the return value, to ensure all the tables were actually created
}
