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
}

std::vector<site> ConfigStorage::get_site_data() const noexcept {
	 const auto result = database->execute("SELECT site_id, site_name from SITES");
	 const auto result_values = result.vector<std::tuple<int, std::string>>();

	 std::vector<site> sites;

	 for (const auto[id_number, name_string] : result_values) {
	 	sites.emplace_back(id_number, name_string);
	 }

	 return sites;
}

std::vector<machine> ConfigStorage::get_machines_for_site(const site& site) const noexcept {
	const auto result = database->execute(find_machine_for_site(site.id));
	const auto result_values = result.vector<std::tuple<int, std::string>>();

	std::vector<machine> machines;
	for (const auto[id, name] : result_values) {
		machines.emplace_back(id, name);
	}

	return machines;
}

std::vector<channel> ConfigStorage::get_channel_information_for_machine(const machine& machine) const noexcept {
	const auto result = database->execute(find_channels_for_machine(machine.id));
	const auto result_values = result.vector<std::tuple<int, std::string, std::string>>();

	std::vector<channel> channels;
	for (const auto[id, name, units] : result_values)
		channels.emplace_back(id, name, units);

	return channels;
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

    const auto result = database->execute(selection_string);
    const auto result_values = result.vector<int>();

    if (!result_values.empty())
    	return result_values.front();

	return -1;
}

std::vector<data_type> ConfigStorage::get_all_types() const noexcept {
	const auto result = database->execute("SELECT types.type_id, types.type_name FROM types");
	const auto result_values = result.vector<std::tuple<int, std::string>>();

	std::vector<data_type> data_types;

	for (const auto[id_number, name_string] : result_values) {
		data_types.emplace_back(id_number, name_string);
	}

	return data_types;
}
