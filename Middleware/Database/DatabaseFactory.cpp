#include "DatabaseFactory.h"

constexpr auto default_database_location = "/Users/thomasmathews/Downloads/VibrationData.db";

std::shared_ptr<sqlite3pp::database> DatabaseFactory::get_database() {
	if (database == nullptr)
		database = std::make_shared<sqlite3pp::database>(default_database_location);

	return database;
}
