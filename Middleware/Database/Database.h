#pragma once

#include <mutex>
#include "IDatabase.h"
#include "Helpers/sqlite3pp.h"

class Database : IDatabase {
public:
	virtual std::vector<site> get_site_data() override;
	std::mutex database_access_mutex;
	void set_up_database();

	sqlite3pp::database database;
	void add_site_with_machines_to_database(const site& s);
};


