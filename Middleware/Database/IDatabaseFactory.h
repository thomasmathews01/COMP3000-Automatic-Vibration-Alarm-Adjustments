#pragma once
#include "Helpers/sqlite3pp.h"

constexpr auto default_database_location = "/Users/thomasmathews/Downloads/VibrationData.db";

class IDatabaseFactory {
public:
	virtual std::shared_ptr<sqlite3pp::database> get_database(const std::string location = default_database_location) = 0;
};