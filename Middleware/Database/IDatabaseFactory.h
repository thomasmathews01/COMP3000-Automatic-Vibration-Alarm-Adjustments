#pragma once
#include <tao/pq.hpp>
#include "Helpers/sqlite3pp.h"

#ifdef WIN32
constexpr auto default_database_location = "C:\\temp\\VibrationData.db";
#else
constexpr auto default_database_location = "postgresql://postgres:password@localhost:5432/postgres";
#endif

class IDatabaseFactory {
public:
	virtual std::shared_ptr<tao::pq::connection> get_database(std::string_view location = default_database_location) = 0;
};