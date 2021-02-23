#pragma once
#include <memory>
#include <string>
#include "../Utils/include_crow.h"
#include <IConfigurationAccess.h>

class TypeInformationRequest {
public:
	static crow::response get_type_information(const crow::request& request, const std::shared_ptr<IConfigurationAccess>& database);
};



