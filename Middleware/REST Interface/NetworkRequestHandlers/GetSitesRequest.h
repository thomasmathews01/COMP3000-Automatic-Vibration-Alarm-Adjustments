#pragma once

#include "../../Database/IDatabase.h"
#include <memory>

class GetSitesRequest {
public:
	static std::string get_sites_info(const std::shared_ptr<IDatabase>& database);
};


