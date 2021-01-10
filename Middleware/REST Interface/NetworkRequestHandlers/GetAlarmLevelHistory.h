#pragma once
#include "../../Database/IDatabase.h"
#include "../../Utils/include_crow.h"
#include <memory>

class GetAlarmLevelHistory {
public:
	static std::string get_alarm_level_history(const crow::request& request, alarmSeverity severity, std::shared_ptr<IDatabase>& database);
};


