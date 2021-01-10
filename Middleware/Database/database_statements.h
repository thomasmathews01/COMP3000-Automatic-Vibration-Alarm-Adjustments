#pragma once

#include <string>

constexpr auto add_new_alarm_settings = "INSERT into alarm_settings(channel_id, type_id, alarm_severity, alarm_threshold_type, custom_fixed_threshold)";



/*
 *  "CREATE TABLE IF NOT EXISTS alarm_settings (\n"
									  "\talarm_settings_id INTEGER PRIMARY KEY,\n"
									  "\tchannel_id INTEGER,\n"
									  "\ttype_id INTEGER, \n"
									  "\talarm_severity INTEGER,\n"
									  "\talarm_threshold_type INTEGER,\n"
									  "\tcustom_fixed_threshold REAL,\n"
									  "\tFOREIGN KEY(channel_id) REFERENCES channels(channel_id),\n"
									  "\tFOREIGN KEY(type_id) REFERENCES types(type_id)\n"
									  ");";
 */