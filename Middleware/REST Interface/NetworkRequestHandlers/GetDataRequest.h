#pragma once

#include "../../Database/IDatabase.h"
#include <crow/crow_all.h>

class GetDataRequest {
public:
	static std::string get_data_points(const crow::request& request, const std::shared_ptr<IDatabase>& database);
};


