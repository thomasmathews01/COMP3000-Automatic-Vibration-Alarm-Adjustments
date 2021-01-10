#pragma once

#include <string>
#include "Helpers/sqlite3pp.h"

class DatabaseInitialiser {
public:
	void intialise_database(sqlite3pp::database& database);
};
