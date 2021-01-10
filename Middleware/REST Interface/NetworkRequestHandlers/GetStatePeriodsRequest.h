#pragma once

#include <memory>
#include <string>
#include "../../Utils/include_crow.h"

#include "../../Database/IDatabase.h"

class GetStatePeriodsRequest {
public:
	static std::string get_state_periods(const crow::request& request, const std::shared_ptr<IDatabase>& database);
};


