#pragma once

#include <memory>
#include <string>

#include <crow/crow_all.h>
#include "../../Database/IDatabase.h"

class AvailableDataTypesRequest {
public:
	static std::string get_available_data_types(const crow::request& request, const std::shared_ptr<IDatabase>& database);
};


