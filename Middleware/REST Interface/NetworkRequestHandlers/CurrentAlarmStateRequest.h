#pragma once
#include "../Model/alarmTypes.h"
#include <memory>
#include <string>

#include <crow/crow_all.h>
#include "../../Database/IDatabase.h"

class CurrentAlarmStateRequest {
public:
	static std::string get_current_alarm_state(const crow::request& request, alarmSeverity severity, std::shared_ptr<IDatabase>& database);
};


