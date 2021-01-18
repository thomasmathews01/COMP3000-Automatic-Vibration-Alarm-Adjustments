#pragma once

#include <string>
#include "Helpers/sqlite3pp.h"
#include "Database.h"

class DatabaseInitialiser {
public:
	void intialise_database(sqlite3pp::database& database);
	void add_alarm_settings_entries_for_all_channels(sqlite3pp::database& database);
	void add_alarm_settings_if_not_exists(const alarm_settings_t settings, sqlite3pp::database& database);
};
