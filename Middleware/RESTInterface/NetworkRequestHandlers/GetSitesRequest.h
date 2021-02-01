#pragma once
#include <memory>
#include <string>
#include "../Utils/include_crow.h"
#include <IConfigurationAccess.h>

class GetSitesRequest {
public:
	static std::string get_sites_info(const std::shared_ptr<IConfigurationAccess>& database);
};


