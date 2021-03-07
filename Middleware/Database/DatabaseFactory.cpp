#include "DatabaseFactory.h"
#include "DatabaseInitialiser.h"
#include <filesystem>

std::shared_ptr<tao::pq::connection> DatabaseFactory::get_database(std::string_view location) {
	//const auto database_exists = std::filesystem::exists(location);
	//assert(database_exists); // We need a database that exists, otherwise everything we do from here on out is pointless.

	if (!database)
		database = DatabaseInitialiser::intialise_database(tao::pq::connection::create(location.data()));

	return database;
}
