#pragma once

#include "../../Types/alarmTypes.h"
#include <memory>
#include "../Utils/include_crow.h"
#include "../../Database/IDatabase.h"

class AlarmActivationsRequest {
public:
	static std::string get_activations(const crow::request& request, alarmSeverity severity, std::shared_ptr<IDatabase>& database);
};


