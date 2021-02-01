#pragma once
#include "IStatsCalcFactory.h"

class StatsCalcFactory : public IStatsCalcFactory {
public:
    std::shared_ptr<IAlarmStatistics>
    get_statistics_calculator(int channel, int type, std::shared_ptr<IStatistics> statistics_storage,
                              std::shared_ptr<IDataAccess> data_storage) final;
};

/*
 * This needs a consolidated interface with alarming. Either we have access to the time in both or neither, or the call site will end up becoming cluttered and confusing.
 */