#pragma once

#include "IDatabaseFactory.h"

class MockDatabaseFactory : public IDatabaseFactory {
public:
	std::shared_ptr<sqlite3pp::database> get_database() final {
		if (database == nullptr)
			database = std::make_shared<sqlite3pp::database>(":memory:");

		return database;
	}

private:
	std::shared_ptr<sqlite3pp::database> database;
};


