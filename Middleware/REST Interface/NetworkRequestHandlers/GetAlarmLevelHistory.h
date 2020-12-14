#pragma once
#include "../../Database/IDatabase.h"
#include <crow/crow_all.h>
#include <memory>

class GetAlarmLevelHistory {
public:
	static std::string get_alarm_level_history(const crow::request& request, alarmSeverity severity, std::shared_ptr<IDatabase>& database);
};


