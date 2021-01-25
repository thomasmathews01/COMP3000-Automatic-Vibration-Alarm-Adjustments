#pragma once
#include <alarmTypes.h>
#include <IStatistics.h>
#include <IDataAccess.h>
#include <IClock.h>
#include "IAlarmStatistics.h"

class AlarmStatistics : public IAlarmStatistics {
public:
	AlarmStatistics(alarm_settings_t settings, std::shared_ptr<IStatistics>  statistics_storage, std::shared_ptr<IDataAccess>  data_storage, std::shared_ptr<IClock>  clock);

	void update() override;
private:
	alarm_settings_t settings;
	std::shared_ptr<IStatistics> statistics_storage;
	std::shared_ptr<IDataAccess> data_storage;
	std::shared_ptr<IClock> clock;

	static void update_statistics(statistics_point_t& cached_values, const std::vector<std::pair<time_point_t, float>>& new_data);
};


