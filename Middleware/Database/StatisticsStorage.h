#pragma once
#include <map>
#include <utility>
#include "IStatistics.h"

class StaticsStorage : public IStatistics {
public:
    statistics_point_t get_last_statistics_calculation(int channel, int type) final;
    void update_last_statistics_calculation(int channel, int type, const statistics_point_t& new_values) final;

private:
    std::map<std::pair<int, int>, statistics_point_t> latest_statistics; // This should be behind a second interface, there is no need to store persistently, might as well calculate them each run and keep in memory for faster access.
};