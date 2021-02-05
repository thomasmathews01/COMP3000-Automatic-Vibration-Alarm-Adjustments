#pragma once

#include <alarmTypes.h>
#include <memory>
#include "../Utils/include_crow.h"
#include <IConfigurationAccess.h>
#include <IAlarmStorage.h>

class AlarmActivationsRequest {
public:
	static std::string get_activations(const crow::request& request, alarmSeverity severity, const std::shared_ptr<IConfigurationAccess>& config_storage,
    const std::shared_ptr<IAlarmStorage>& alarm_storage);
};


