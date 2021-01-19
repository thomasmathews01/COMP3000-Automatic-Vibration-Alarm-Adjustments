#pragma once

#include "IDatabaseFactory.h"

class DatabaseFactory : public IDatabaseFactory {
public:
	std::shared_ptr<sqlite3pp::database> get_database() final;

private:
	std::shared_ptr<sqlite3pp::database> database;
};


