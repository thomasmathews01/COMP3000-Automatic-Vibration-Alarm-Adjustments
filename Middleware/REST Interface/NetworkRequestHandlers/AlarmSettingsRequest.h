#pragma once
#include "../../Utils/include_crow.h"
#include <string>
#include <memory>
#include "../../Database/IDatabase.h"

class AlarmSettingsRequest {
public:
	static std::string alarm_settings(const crow::request& request, alarmSeverity severity, std::shared_ptr<IDatabase>& database);
};


