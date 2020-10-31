#pragma once


#include "../REST Interface/Model/types.h"

class IDatabase {
public:
	virtual std::vector<site> get_site_data() = 0;
};


