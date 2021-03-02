#pragma once
#include "Helpers/sqlite3pp.h"

#ifdef WIN32
constexpr auto default_database_location = "C:\\temp\\VibrationData.db";
#else
constexpr auto default_database_location = "/Users/thomasmathews/Downloads/VibrationData.db";
#endif


class IDatabaseFactory {
public:
	virtual std::shared_ptr<sqlite3pp::database> get_database(std::string_view location = default_database_location) = 0;
};