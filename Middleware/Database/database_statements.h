#pragma once

#include <string>

constexpr auto create_types = "CREATE TABLE IF NOT EXISTS types (type_id INTEGER PRIMARY KEY AUTOINCREMENT, type_name VARCHAR(30))";
constexpr auto create_sites = "CREATE TABLE IF NOT EXISTS sites (site_id INTEGER PRIMARY KEY AUTOINCREMENT, site_name varchar(40))";
constexpr auto create_machines = "CREATE TABLE IF NOT EXISTS machines (machine_id INTEGER PRIMARY KEY AUTOINCREMENT, machine_name varchar(40), site_id INTEGER, FOREIGN KEY(site_id) REFERENCES sites(side_id))";
constexpr auto create_channels = "CREATE TABLE IF NOT EXISTS channels (channel_id INTEGER PRIMARY KEY AUTOINCREMENT, channel_name VARCHAR(40), channel_units VARCHAR(10), machine_id INTEGER, FOREIGN KEY(machine_id) REFERENCES machines(machine_id))";
constexpr auto create_data = "CREATE TABLE IF NOT EXISTS data (type_id INTEGER, channel_id INTEGER, time_since_epoch INTEGER, value REAL, PRIMARY KEY (type_id, time_since_epoch, channel_id))";

constexpr auto setup_alarm_settings = "CREATE TABLE IF NOT EXISTS alarm_settings (\n"
									  "channel_id INTEGER,\n"
									  "type_id INTEGER, \n"
									  "alarm_severity INTEGER,\n"
									  "alarm_threshold_type INTEGER,\n"
									  "custom_fixed_threshold REAL,\n"
									  "FOREIGN KEY(channel_id) REFERENCES channels(channel_id),\n"
									  "FOREIGN KEY(type_id) REFERENCES types(type_id),\n"
									  "PRIMARY KEY(channel_id, type_id, alarm_severity)\n"
									  ")";

constexpr auto setup_alarm_levels = "CREATE TABLE IF NOT EXISTS alarm_levels (\n"
									"channel_id INTEGER,\n"
									"type_id INTEGER, \n"
									"alarm_severity INTEGER,\n"
									"time_since_epoch INTEGER,\n"
									"level REAL,\n"
									"FOREIGN KEY(channel_id) REFERENCES channels(channel_id),\n"
									"FOREIGN KEY(type_id) REFERENCES types(type_id),\n"
									"PRIMARY KEY(channel_id, type_id, alarm_severity, time_since_epoch)\n"
									");";

constexpr auto setup_alarm_activation_changes = "CREATE TABLE IF NOT EXISTS alarm_activation_changes (\n"
												"channel_id INTEGER,\n"
												"type_id INTEGER, \n"
												"alarm_severity INTEGER,\n"
												"became_active BOOLEAN ,\n"
												"time_since_epoch INTEGER,\n"
												"FOREIGN KEY(channel_id) REFERENCES channels(channel_id),\n"
												"FOREIGN KEY(type_id) REFERENCES types(type_id),\n"
												"PRIMARY KEY(channel_id, type_id, alarm_severity)\n"
												");";

constexpr auto setup_state_settings = "CREATE TABLE IF NOT EXISTS state_settings (\n"
									  "state_id INTEGER PRIMARY KEY,\n"
									  "machine_id INTEGER,\n"
									  "state_name TEXT,\n"
									  "FOREIGN KEY(machine_id) REFERENCES machines(machine_id)\n"
									  ");";

constexpr auto setup_state_changes = "CREATE TABLE IF NOT EXISTS state_changes (\n"
									 "machine_id INTEGER,\n"
									 "new_state_id INTEGER,\n"
									 "time_since_epoch INTEGER,\n"
									 "FOREIGN KEY (new_state_id) REFERENCES state_settings(state_id),\n"
									 "FOREIGN KEY (machine_id) REFERENCES machines(machine_id)\n"
									 "\n"
									 ");";

constexpr auto select_all_unique_channels = "SELECT DISTINCT channels.channel_id from channels";

constexpr auto get_all_types_with_data_associated_with_them = "SELECT DISTINCT type_id FROM data";

constexpr auto add_alarm_settings = "INSERT OR IGNORE INTO alarm_settings (channel_id, type_id,  alarm_severity, alarm_threshold_type,  custom_fixed_threshold)\n"
									"VALUES (:channel, :type, :severity, :threshold_type, :custom_threshold)";

constexpr auto add_alarm_level_history_point = "INSERT INTO alarm_levels (channel_id, type_id,  alarm_severity, level,  time_since_epoch)\n"
											   "VALUES (:channel, :type, :severity, :level, :time_since_epoch)";

constexpr auto modify_alarm_settings = "UPDATE alarm_settings SET alarm_threshold_type = :threshold_type, custom_fixed_threshold = :fixed_threshold "
									   "WHERE channel_id = :channel AND type_id = :type AND alarm_severity = :severity";

constexpr auto find_channel_by_machine_id = "SELECT channel_id, channel_name, channel_units FROM channels WHERE machine_id = ";
constexpr auto find_channels_for_machine = [](const auto channel)
{
	return find_channel_by_machine_id + std::to_string(channel);
};

constexpr auto find_machine_by_site_id = "SELECT machine_id, machine_name from machines WHERE site_id = ";
constexpr auto find_machine_for_site = [](const auto machine)
{
	return find_machine_by_site_id + std::to_string(machine);
};
