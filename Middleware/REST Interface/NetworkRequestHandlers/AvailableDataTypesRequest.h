#pragma once

#include <memory>
#include "../../Database/IDatabase.h"
#include <crow/crow_all.h>

class AvailableDataTypesRequest {
public:
	static std::string get_available_data_types(const crow::request& request, const std::shared_ptr<IDatabase>& database);
};


