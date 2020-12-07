#pragma once

#include "../../Database/IDatabase.h"
#include <crow/crow_all.h>

class GetStatePeriodsRequest {
public:
	static std::string get_state_periods(const crow::request& request, const std::shared_ptr<IDatabase>& database);
};


