#pragma once
#include <memory>
#include <alarmTypes.h>
#include <IStatistics.h>
#include <IDataAccess.h>
#include <IClock.h>
#include "IAlarmStatistics.h"

class AlarmStatistics : public IAlarmStatistics {
public:
	AlarmStatistics(alarm_settings_t settings, std::shared_ptr<IStatistics>  statistics_storage, std::shared_ptr<IDataAccess>  data_storage);

	void update(const time_point_t& time) override;
private:
	alarm_settings_t settings;
	std::shared_ptr<IStatistics> statistics_storage;
	std::shared_ptr<IDataAccess> data_storage;

	static void update_statistics(statistics_point_t& cached_values, const std::vector<std::pair<time_point_t, float>>& new_data);
};


