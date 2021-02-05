#pragma once
#include "../Utils/include_crow.h"
#include <memory>
#include <IAlarmStorage.h>

class GetAlarmLevelHistory {
public:
	static std::string get_alarm_level_history(const crow::request& request, alarmSeverity severity, const std::shared_ptr<IAlarmStorage>& database);
};


