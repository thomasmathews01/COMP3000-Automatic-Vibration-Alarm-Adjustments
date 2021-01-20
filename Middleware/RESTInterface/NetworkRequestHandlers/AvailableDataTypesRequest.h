#pragma once

#include <memory>
#include <string>

#include "../Utils/include_crow.h"
#include "../../Database/IDatabase.h"

class AvailableDataTypesRequest {
public:
	static std::string get_available_data_types(const crow::request& request, const std::shared_ptr<IDatabase>& database);
};


