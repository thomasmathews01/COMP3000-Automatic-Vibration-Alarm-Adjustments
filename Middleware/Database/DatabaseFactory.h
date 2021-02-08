#pragma once
#include "IDatabaseFactory.h"
#include "DatabaseInitialiser.h"

class DatabaseFactory : public IDatabaseFactory {
public:
	explicit DatabaseFactory(std::shared_ptr<sqlite3pp::database>&& default_database) : database(DatabaseInitialiser::intialise_database(std::move(default_database))) {}
	DatabaseFactory() = default;
	std::shared_ptr<sqlite3pp::database> get_database(std::string_view location) final;

private:
	std::shared_ptr<sqlite3pp::database> database;
};


