#include "StatsCalcFactory.h"
#include "AlarmStatistics.h"

std::shared_ptr<IAlarmStatistics> StatsCalcFactory::get_statistics_calculator(int channel, int type, std::shared_ptr<IStatistics> statistics_storage,
                                            std::shared_ptr<IDataAccess> data_storage) {
    return std::make_shared<AlarmStatistics>(channel, type, statistics_storage, data_storage);
}
