#include "GetAlarmLevelHistory.h"
#include "../Utils/CrowUtils.h"

crow::response GetAlarmLevelHistory::get_alarm_level_history(const crow::request& request, alarmSeverity severity, const std::shared_ptr<IAlarmStorage>& database) {
	return CrowUtils::add_cors_headers(crow::response("Not yet supported"));
}
