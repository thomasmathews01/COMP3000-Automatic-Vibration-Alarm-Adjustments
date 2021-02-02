#include "StatisticsStorage.h"

statistics_point_t StaticsStorage::get_last_statistics_calculation(int channel, int type) {
    const auto map_index = std::make_pair(channel, type);
    const auto matching_statistic = latest_statistics.emplace(map_index, statistics_point_t());

    return matching_statistic.first->second;
}

void StaticsStorage::update_last_statistics_calculation(int channel, int type, const statistics_point_t& new_values) {
    latest_statistics.at({channel, type}) = new_values;
}
