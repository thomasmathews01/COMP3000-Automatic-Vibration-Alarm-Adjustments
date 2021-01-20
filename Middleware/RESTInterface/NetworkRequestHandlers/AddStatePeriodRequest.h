#pragma once

#include <memory>
#include <string>
#include "../Utils/include_crow.h"
#include "../../Database/IDatabase.h"

class AddStatePeriodRequest {
public:
	static std::string add_state_period(const crow::request& request, std::shared_ptr<IDatabase>& database);
};


