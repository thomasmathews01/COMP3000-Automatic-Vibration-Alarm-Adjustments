#pragma once

#include <memory>
#include <string>

#include <crow/crow_all.h>
#include "../../Database/IDatabase.h"

class GetStatePeriodsRequest {
public:
	static std::string get_state_periods(const crow::request& request, const std::shared_ptr<IDatabase>& database);
};


