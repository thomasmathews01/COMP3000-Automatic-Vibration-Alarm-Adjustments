#pragma once

#include "../crow.h"
#include "../../Database/IDatabase.h"

class SiteInformationRequest {
public:
	static std::string get_site_information(const crow::request& request, const std::shared_ptr<IDatabase>& database);
};

