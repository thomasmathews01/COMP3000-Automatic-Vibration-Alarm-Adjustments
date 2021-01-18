#pragma once

#include "IDatabaseFactory.h"

class DatabaseFactory : public IDatabaseFactory {
public:
	std::string get_database() final;
};


