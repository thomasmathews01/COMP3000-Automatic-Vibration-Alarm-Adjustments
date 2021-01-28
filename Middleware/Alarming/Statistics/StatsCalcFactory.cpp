#include "StatsCalcFactory.h"
#include "AlarmStatistics.h"

std::shared_ptr<IAlarmStatistics> StatsCalcFactory::get_statistics_calculator(alarm_settings_t settings, std::shared_ptr<IStatistics> statistics_storage,
                                            std::shared_ptr<IDataAccess> data_storage) {
    switch (settings.threshold)
    {
        case alarmThreshold::automaticBasic:
            return std::make_shared<AlarmStatistics>(settings, statistics_storage, data_storage);
        case alarmThreshold::automaticStateBased:
            throw std::invalid_argument("");
        case alarmThreshold::ISOLevel:
        case alarmThreshold::Custom:
            // Don't require statistics, both rely on fixed levels.
            break;
    }
}

/*
 * How do we deal with the change of alarm settings? Handle all statistics on all channels always?
 * Minor excess processing, but plenty of time for a simple m/stdd op.
 * In which case the whole factory pattern is BS anyway, since we need to have a
 */