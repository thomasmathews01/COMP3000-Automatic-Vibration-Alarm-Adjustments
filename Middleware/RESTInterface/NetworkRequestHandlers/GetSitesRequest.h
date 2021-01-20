#pragma once
#include <memory>
#include <string>
#include "../Utils/include_crow.h"
#include "../../Database/IDatabase.h"

class GetSitesRequest {
public:
	static std::string get_sites_info(const std::shared_ptr<IDatabase>& database);
};


