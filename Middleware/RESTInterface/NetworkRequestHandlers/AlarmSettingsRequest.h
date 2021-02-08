#pragma once
#include "../Utils/include_crow.h"
#include <string>
#include <memory>
#include <IAlarmStorage.h>

class AlarmSettingsRequest {
public:
	static crow::response alarm_settings(const crow::request& request, alarmSeverity severity, const std::shared_ptr<IAlarmStorage>& database);
};


