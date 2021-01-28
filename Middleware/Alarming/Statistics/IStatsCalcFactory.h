#pragma once
#include "IAlarmStatistics.h"
#include <memory>
#include <IDataAccess.h>
#include <IStatistics.h>

class IStatsCalcFactory {
public:
    virtual std::shared_ptr<IAlarmStatistics> get_statistics_calculator(alarm_settings_t settings, std::shared_ptr<IStatistics>  statistics_storage, std::shared_ptr<IDataAccess>  data_storage) = 0;
};
