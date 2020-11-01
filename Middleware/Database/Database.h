#pragma once

#include <mutex>
#include "IDatabase.h"
#include "Helpers/sqlite3pp.h"

constexpr auto default_database_location = "/Users/thomasmathews/Documents/VibrationData.db";

class Database : public IDatabase {
public:
	std::vector<site> get_site_data() override;
	std::mutex database_access_mutex;
	void set_up_database_connection();

	sqlite3pp::database database;
	void populate_sites_machine_information(site& site) override;
	void populate_channel_information_for_a_machine(machine& machine) override;
};


