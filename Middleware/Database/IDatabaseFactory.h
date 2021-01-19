#pragma once

#include "Helpers/sqlite3pp.h"

class IDatabaseFactory {
public:
	virtual std::shared_ptr<sqlite3pp::database> get_database() = 0;
};