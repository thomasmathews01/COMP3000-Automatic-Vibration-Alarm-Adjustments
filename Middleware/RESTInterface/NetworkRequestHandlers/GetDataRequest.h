#pragma once

#include <memory>
#include <string>
#include "../Utils/include_crow.h"

#include "../../Database/IDatabase.h"

class GetDataRequest {
public:
	static std::string get_data_points(const crow::request& request, const std::shared_ptr<IDatabase>& database);
};


