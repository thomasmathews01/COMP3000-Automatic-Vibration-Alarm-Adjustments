#pragma once
#include <memory>
#include <string>
#include "../Utils/include_crow.h"
#include <IConfigurationAccess.h>

class MachineInformationRequest {
public:
	static crow::response get_machine_information(const crow::request& request, const std::shared_ptr<IConfigurationAccess>& database);
};


