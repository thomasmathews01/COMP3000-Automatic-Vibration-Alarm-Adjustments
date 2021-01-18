#pragma once

#include <string>

constexpr auto create_types = "CREATE TABLE types (type_id INTEGER PRIMARY KEY AUTOINCREMENT, type_name VARCHAR(30))";
constexpr auto create_sites = "CREATE TABLE sites (site_id INTEGER PRIMARY KEY AUTOINCREMENT, site_name varchar(40))";
constexpr auto create_machines = "CREATE TABLE machines (machine_id INTEGER PRIMARY KEY AUTOINCREMENT, machine_name varchar(40), site_id INTEGER, FOREIGN KEY(site_id) REFERENCES sites(side_id))";
constexpr auto create_channels = "CREATE TABLE channels (channel_id INTEGER PRIMARY KEY AUTOINCREMENT, channel_name VARCHAR(40), channel_units VARCHAR(10), machine_id INTEGER, FOREIGN KEY(machine_id) REFERENCES machines(machine_id))";
constexpr auto create_data = "CREATE TABLE data (type_id INTEGER, channel_id INTEGER, time_since_epoch INTEGER, value REAL, PRIMARY KEY (type_id, time_since_epoch, channel_id))";

constexpr auto add_new_alarm_settings = "INSERT into alarm_settings(channel_id, type_id, alarm_severity, alarm_threshold_type, custom_fixed_threshold)";

constexpr auto setup_alarm_settings = "CREATE TABLE IF NOT EXISTS alarm_settings (\n"
									  "channel_id INTEGER,\n"
									  "type_id INTEGER, \n"
									  "alarm_severity INTEGER,\n"
									  "alarm_threshold_type INTEGER,\n"
									  "custom_fixed_threshold REAL,\n"
									  "FOREIGN KEY(channel_id) REFERENCES channels(channel_id),\n"
									  "FOREIGN KEY(type_id) REFERENCES types(type_id),\n"
									  "PRIMARY KEY (channel_id, type_id, alarm_severity)\n"
									  ")";

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

constexpr auto select_all_unique_channels = "SELECT DISTINCT channels.channel_id from channels";

constexpr auto get_all_types_with_data_associated_with_them = "SELECT DISTINCT type_id FROM data";

constexpr auto add_alarm_settings = "INSERT OR IGNORE INTO alarm_settings (alarm_settings_id,  channel_id, type_id,  alarm_severity, alarm_threshold_type,  custom_fixed_threshold)\n"
									"VALUES (:id, :channel, :type, :severity, :threshold_type, :custom_threshold)";

constexpr auto modify_alarm_settings = "UPDATE alarm_settings SET alarm_threshold_type = :threshold_type, custom_fixed_threshold = :fixed_threshold "
									   "WHERE channel_id = :channel AND type_id = :type AND alarm_severity = :severity";

