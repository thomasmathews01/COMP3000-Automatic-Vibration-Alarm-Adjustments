#include "DatabaseFactory.h"

std::shared_ptr<sqlite3pp::database> DatabaseFactory::get_database(const std::string location) {
	if (database == nullptr)
		database = std::make_shared<sqlite3pp::database>(location.c_str());

	return database;
}
