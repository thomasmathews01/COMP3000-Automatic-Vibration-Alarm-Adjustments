#include "ConfigStorage.h"
#include <string>
#include <algorithm>
#include <range/v3/all.hpp>
#include "database_statements.h"

namespace {
    int seconds_since_epoch(const time_point_t t) noexcept {
        return duration_cast<seconds>(t.time_since_epoch()).count();
    }

    std::string seconds_since_epoch_str(const time_point_t t) noexcept {
        return std::to_string(seconds_since_epoch(t));
    }

    template<class T, class F>
    std::vector<T> get_query_results(const char *statement, const std::shared_ptr<sqlite3pp::database>& database, F&& func) noexcept {
        std::vector<T> result;

        auto query_results = sqlite3pp::query(*database, statement);
        std::transform(query_results.begin(), query_results.end(), std::back_inserter(result), func);

        return result;
    }
}

std::vector<site> ConfigStorage::get_site_data() const noexcept {
    return get_query_results<site>("SELECT site_id, site_name from SITES", database, [](const auto& row) {
        const auto[id_number, name_string] = row.template get_columns<int, const char *>(0, 1);
        return site(id_number, name_string);
    });
}

std::vector<machine> ConfigStorage::get_machines_for_site(const site& site) const noexcept {
    return get_query_results<machine>(find_machine_for_site(site.id).c_str(), database, [](const auto& row) {
        const auto[id, name] = row.template get_columns<int, const char *>(0, 1);

        return machine(id, name);
    });
}

std::vector<channel> ConfigStorage::get_channel_information_for_machine(const machine& machine) const noexcept {
    return get_query_results<channel>(find_channels_for_machine(machine.id).c_str(), database, [](const auto& row) {
        const auto[id, name, units] = row.template get_columns<int, const char *, const char *>(0, 1, 2);

        return channel(id, name, units);
    });
}

std::vector<channel> ConfigStorage::get_all_channels() const noexcept {
    std::vector<channel> channels;

    for (const auto& site : get_site_data())
        for (const auto& machine : get_machines_for_site(site))
            channels |= ranges::actions::push_back(get_channel_information_for_machine(machine));

    return channels;
}

int ConfigStorage::get_machine_id_from_channel_id(int channel_id) const noexcept {
    const auto selection_string = "SELECT machine_id from channels where channel_id = " + std::to_string(channel_id);

    for (const auto& row : sqlite3pp::query(*database, selection_string.c_str())) {
        const auto[machine_id] = row.template get_columns<int>(0);
        return machine_id;
    }

    return -1;
}
