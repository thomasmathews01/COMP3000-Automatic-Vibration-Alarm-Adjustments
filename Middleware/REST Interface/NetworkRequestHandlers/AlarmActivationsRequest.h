#pragma once

#include "../Model/alarmTypes.h"
#include <memory>
#include <crow/crow_all.h>
#include "../../Database/IDatabase.h"

class AlarmActivationsRequest {
public:
	static std::string get_activations(const crow::request& request, alarmSeverity severity, std::shared_ptr<IDatabase>& database);
};


