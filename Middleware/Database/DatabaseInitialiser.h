#pragma once

#include <string>
#include "Helpers/sqlite3pp.h"
#include "Database.h"

class DatabaseInitialiser {
public:
	static std::shared_ptr<tao::pq::connection> intialise_database(std::shared_ptr<tao::pq::connection>&& database);
	static void add_alarm_settings_entries_for_all_channels(const std::shared_ptr<tao::pq::connection>& database);
	static void add_alarm_settings_if_not_exists(const alarm_settings_t& settings, const std::shared_ptr<tao::pq::connection>& database);
};
