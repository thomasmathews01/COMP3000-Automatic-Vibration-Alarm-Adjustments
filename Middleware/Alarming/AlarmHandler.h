#pragma once

#include "IClock.h"
#include <memory>
#include "IDatabase.h"
#include "Calculators/IAlarmCalcFactory.h"
#include "Statistics/IStatsCalcFactory.h"

class AlarmHandler {
public:
	AlarmHandler(std::shared_ptr<IClock> internal_clock, std::shared_ptr<IDatabase> database, std::shared_ptr<IAlarmCalcFactory> factory, std::shared_ptr<IStatistics> statistics_storage, std::shared_ptr<IDataAccess> data_storage, std::shared_ptr<IStatsCalcFactory> statistics_factory);
	void process();

private:
	std::shared_ptr<IClock> internal_clock;
	std::shared_ptr<IDatabase> database;
    std::shared_ptr<IStatistics> statistics_storage;
    std::shared_ptr<IDataAccess> data_storage;
	std::shared_ptr<IAlarmCalcFactory> alarm_factory;
	std::shared_ptr<IStatsCalcFactory> statistics_factory;
	std::vector<std::shared_ptr<IAlarmCalc>> alarms;
	std::vector<std::shared_ptr<IAlarmStatistics>> statistics;

	void load_alarms();
	void load_statistics();
};


