#pragma once

#include <string>
#include "Helpers/sqlite3pp.h"
#include "Database.h"

class DatabaseInitialiser {
public:
	void intialise_database(std::shared_ptr<sqlite3pp::database> database);
	void add_alarm_settings_entries_for_all_channels(std::shared_ptr<sqlite3pp::database> database);
	void add_alarm_settings_if_not_exists(const alarm_settings_t settings, std::shared_ptr<sqlite3pp::database> database);
};
