#pragma once

#include "Helpers/sqlite3pp.h"

class IDatabaseFactory {
public:
	virtual std::string get_database() = 0;
};