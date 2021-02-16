#pragma once
#include <memory>
#include <string>
#include "../Utils/include_crow.h"
#include <alarmTypes.h>
#include <IAlarmStorage.h>
#include <IConfigurationAccess.h>


class CurrentAlarmStateRequest {
public:
	static crow::response get_current_alarm_state(const crow::request& request, const std::shared_ptr<IConfigurationAccess>& config_storage, const std::shared_ptr<IAlarmStorage>& alarm_storage);
};


