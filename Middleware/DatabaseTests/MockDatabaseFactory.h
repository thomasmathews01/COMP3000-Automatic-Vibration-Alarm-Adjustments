#pragma once

#include "IDatabaseFactory.h"

class MockDatabaseFactory : public IDatabaseFactory {
	std::string get_database() final {
		return ":memory:";
	}
};


