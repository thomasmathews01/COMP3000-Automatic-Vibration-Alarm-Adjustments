#pragma once

#include "../../Database/IDatabase.h"
#include <crow/crow_all.h>

class MachineInformationRequest {
public:
	static std::string get_machine_information(const crow::request& request, const std::shared_ptr<IDatabase>& database);
};


