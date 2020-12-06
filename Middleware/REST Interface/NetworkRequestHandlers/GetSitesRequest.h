#pragma once

#include "../../Database/IDatabase.h"

class GetSitesRequest {
public:
	static std::string get_sites_info(const std::shared_ptr<IDatabase>& database);
};


