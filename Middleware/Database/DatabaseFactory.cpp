#include "DatabaseFactory.h"
#include "DatabaseInitialiser.h"

std::shared_ptr<sqlite3pp::database> DatabaseFactory::get_database(std::string_view location) {
	if (!database)
		database = DatabaseInitialiser::intialise_database(std::make_shared<sqlite3pp::database>(location.data()));

	return database;
}
