#pragma once

#include <memory>
#include <string>
#include "../Utils/include_crow.h"

#include "../../Database/IDatabase.h"

class MachineInformationRequest {
public:
	static std::string get_machine_information(const crow::request& request, const std::shared_ptr<IDatabase>& database);
};


