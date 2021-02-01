#pragma once

#include <memory>
#include <alarmTypes.h>
#include <IStatistics.h>
#include <IDataAccess.h>
#include <IClock.h>
#include "IAlarmStatistics.h"
#include "IAlarmStorage.h"

class AlarmStatistics : public IAlarmStatistics {
public:
    AlarmStatistics(int channel, int type, std::shared_ptr<IStatistics> statistics_storage, std::shared_ptr<IDataAccess> data_storage);

    void update(const time_point_t& time) override;

private:
    int channel, type;
    std::shared_ptr<IAlarmStorage> alarm_storage;
    std::shared_ptr<IStatistics> statistics_storage;
    std::shared_ptr<IDataAccess> data_storage;

    static statistics_point_t
    update_statistics(const statistics_point_t& cached_values, const std::vector<std::pair<time_point_t, float>>& new_data);
};


