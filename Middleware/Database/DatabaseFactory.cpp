#include "DatabaseFactory.h"

constexpr auto default_database_location = "/Users/thomasmathews/Downloads/VibrationData.db";

std::string DatabaseFactory::get_database() {
	return default_database_location;
}
