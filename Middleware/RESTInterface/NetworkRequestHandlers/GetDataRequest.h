#pragma once

#include <memory>
#include <string>
#include "../Utils/include_crow.h"
#include <IDataAccess.h>

class GetDataRequest {
public:
	static crow::response get_data_points(const crow::request& request, const std::shared_ptr<IDataAccess>& database);
};


