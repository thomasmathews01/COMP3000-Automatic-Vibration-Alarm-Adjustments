#pragma once

#include "../../Database/IDatabase.h"
#include <crow/crow_all.h>

class AddStatePeriodRequest {
public:
	static std::string add_state_period(const crow::request& request, std::shared_ptr<IDatabase>& database);
};


