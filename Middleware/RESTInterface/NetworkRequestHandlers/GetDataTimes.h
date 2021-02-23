#pragma once
#include <memory>
#include <string>

#include "../Utils/include_crow.h"
#include <IDataAccess.h>

class GetDataTimes {
public:
	static crow::response get_earliest_point(const crow::request& request, const std::shared_ptr<IDataAccess>& database);
	static crow::response get_latest_point(const crow::request& request, const std::shared_ptr<IDataAccess>& database);
};

