#include "DataStorage.h"
#include <algorithm>

using namespace std::string_literals;

namespace {
    int seconds_since_epoch(const time_point_t t) noexcept {
        return duration_cast<seconds>(t.time_since_epoch()).count();
    }

    std::string seconds_since_epoch_str(const time_point_t t) noexcept {
        return std::to_string(seconds_since_epoch(t));
    }

    template<class T, class F>
    std::vector<T>
    get_query_results(const char *statement, const std::shared_ptr<sqlite3pp::database>& database, F&& func) noexcept {
        std::vector<T> result;

        auto query_results = sqlite3pp::query(*database, statement);
        std::transform(query_results.begin(), query_results.end(), std::back_inserter(result), func);

        return result;
    }
}

std::vector<std::pair<time_point_t, float>>
DataStorage::get_data(int channel, int type, time_point_t start, time_point_t finish) const noexcept {
    const auto statement = "SELECT time_since_epoch, value FROM data"s +
                           " WHERE type_id =  " + std::to_string(type) +
                           " AND time_since_epoch BETWEEN " + seconds_since_epoch_str(start) + " AND " +
                           std::to_string(seconds_since_epoch(finish)) +
                           " AND channel_id = " + std::to_string(channel) + " ";

    return get_query_results<std::pair<time_point_t, float>>(statement.c_str(), database, [](const auto& row) {
        const auto[epoch_time, value] = row.template get_columns<int, float>(0, 1);

        return std::make_pair(time_point_t(seconds(epoch_time)), value);
    });
}

std::vector<std::pair<int, std::string>> DataStorage::get_data_types_available_for_channel(int channel_id) const noexcept {
    const auto statement =
            "SELECT DISTINCT types.type_id, types.type_name FROM data INNER JOIN types on types.type_id = data.type_id WHERE channel_id = " +
            std::to_string(channel_id);

    //const auto statement = "SELECT DISTINCT type_id, type_name FROM types";

    return get_query_results<std::pair<int, std::string>>(statement.c_str(), database, [](const auto& row)  {
        const auto[type_id, type_name] = row.template get_columns<int, const char *>(0, 1);
        return std::make_pair(type_id, type_name);
    });
}

time_point_t DataStorage::get_earliest_data_point_for_machine(int machine_id) const noexcept {
    const auto selection_string =
            "SELECT min(data.time_since_epoch) from data inner join channels on data.channel_id = channels.channel_id "
            "inner join machines on channels.machine_id = machines.machine_id where machines.machine_id = " +
            std::to_string(machine_id);
    for (const auto& row : sqlite3pp::query(*database, selection_string.c_str())) {
        const auto[seconds_since_epoch] = row.template get_columns<int>(0);
        return time_point_t(seconds(seconds_since_epoch));
    }

    return time_point_t(0s);
}


std::pair<time_point_t, float> DataStorage::get_last_data_point_before(int channel, int type, time_point_t time) const noexcept {
    const auto statement = "SELECT time_since_epoch, value FROM data"s +
                           " WHERE type_id =  " + std::to_string(type) +
                           " AND time_since_epoch <= " + seconds_since_epoch_str(time) +
                           " AND channel_id = " + std::to_string(channel) +
                           " ORDER BY time_since_epoch DESC" +
                           " LIMIT 1";

    auto query_results = sqlite3pp::query(*database, statement.c_str());
    // TODO: What if we don't find any results? Should probably check and at least return an optional. We don't want async exception throwing, this code needs to be fast and non blocking as much as possible.

    const auto first_row = query_results.begin().operator*();
    const auto[epoch_time, value] = first_row.get_columns<int, float>(0, 1);

    return std::make_pair(time_point_t(seconds(epoch_time)), value);
}
std::vector<float> DataStorage::get_data_points_only(int channel, int type, time_point_t start, time_point_t finish) const noexcept {
    const auto statement = "SELECT value FROM data"s +
                           " WHERE type_id =  " + std::to_string(type) +
                           " AND channel_id = " + std::to_string(channel) +
                           " AND time_since_epoch BETWEEN " + seconds_since_epoch_str(start + 1s) + " AND " +
                           std::to_string(seconds_since_epoch(finish));

    return get_query_results<float>(statement.c_str(), database, [](const auto& row) {
        const auto[value] = row.template get_columns<float>(0);
        return value;
    });
}

std::vector<int> DataStorage::get_all_data_types() const noexcept {
    const auto statement = "SELECT DISTINCT type_id FROM types";

    return get_query_results<int>(statement, database, [](const auto& row) {
        const auto[type_id] = row.template get_columns<int>(0);
        return type_id;
    });
}

