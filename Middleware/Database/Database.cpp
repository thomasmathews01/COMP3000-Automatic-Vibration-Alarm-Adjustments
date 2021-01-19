#include "Database.h"
#include "DatabaseInitialiser.h"
#include "database_statements.h"
#include <range/v3/all.hpp>

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace std::string_literals;
using namespace ranges;

namespace
{
	int seconds_since_epoch(const time_point_t t) {
		return duration_cast<seconds>(t.time_since_epoch()).count();
	}

	std::string seconds_since_epoch_str(const time_point_t t) {
		return std::to_string(seconds_since_epoch(t));
	}

	template<class T, class F>
	std::vector<T> get_query_results(const char* statement, const std::shared_ptr<sqlite3pp::database>& database, F&& func) {
		std::vector<T> result;

		auto query_results = sqlite3pp::query(*database, statement);
		std::transform(query_results.begin(), query_results.end(), std::back_inserter(result), func);

		return result;
	}
}

std::vector<site> Database::get_site_data() {
	std::lock_guard guard(database_access_mutex);

	return get_query_results<site>("SELECT site_id, site_name from SITES", database, [](const auto row)
	{
		const auto[id_number, name_string] = row.get_columns<int, const char*>(0, 1);
		return site(id_number, name_string);
	});
}

std::vector<machine> Database::get_machines_for_site(const site& site) {
	std::lock_guard guard(database_access_mutex);

	return get_query_results<machine>(find_machine_for_site(site.id).c_str(), database, [](const auto row)
	{
		const auto[id, name] = row.get_columns<int, const char*>(0, 1);

		return machine(id, name);
	});
}

std::vector<channel> Database::get_channel_information_for_machine(const machine& machine) {
	std::lock_guard guard(database_access_mutex);

	return get_query_results<channel>(find_channels_for_machine(machine.id).c_str(), database, [](const auto row)
	{
		const auto[id, name, units] = row.get_columns<int, const char*, const char*>(0, 1, 2);

		return channel(id, name, units);
	});
}

std::vector<std::pair<time_point_t, float>> Database::get_data(int channel, int type, time_point_t start, time_point_t finish) {
	std::lock_guard guard(database_access_mutex);

	const auto statement = "SELECT time_since_epoch, value FROM data"s
							   " WHERE type_id =  " + std::to_string(type) +
						   " AND time_since_epoch BETWEEN " + seconds_since_epoch_str(start) + " AND " + std::to_string(seconds_since_epoch(finish)) +
						   " AND channel_id = " + std::to_string(channel) + " ";

	return get_query_results<std::pair<time_point_t, float>>(statement.c_str(), database, [](const auto row)
	{
		const auto[epoch_time, value] = row.get_columns<int, float>(0, 1);

		return std::make_pair(time_point_t(seconds(epoch_time)), value);
	});
}

std::vector<std::pair<int, std::string>> Database::get_data_types_available_for_channel(int channel_id) {
	return {};
}

int Database::get_machine_id_from_channel_id(int channel_id) {
	std::lock_guard guard(database_access_mutex);

	const auto selection_string = "SELECT machine_id from channels where channel_id = " + std::to_string(channel_id);

	for (const auto& row : sqlite3pp::query(*database, selection_string.c_str())) {
		const auto[machine_id] = row.get_columns<int>(0);
		return machine_id;
	}

	return -1;
}

time_point_t Database::get_earliest_data_point_for_machine(int machine_id) {
	//const auto selection_string = "SELECT "
	return time_point_t();
}

std::vector<state_change_t> Database::get_state_changes_for_machine(int machine_id) {
	return std::vector<state_change_t>();
}

void Database::add_new_state_period(int machine_id, state_period_t state_period) {

}

std::vector<alarm_activation_t> Database::get_activations_for_machine(int machine_id) {
	return std::vector<alarm_activation_t>();
}

std::vector<alarm_settings_t> Database::get_alarm_settings_for_machine(int machine_id) {
	return std::vector<alarm_settings_t>();
}

bool Database::update_alarm_setting(const alarm_settings_t& new_setting) {
	std::lock_guard guard(database_access_mutex);

	sqlite3pp::command cmd(*database, modify_alarm_settings);

	cmd.bind(":type", std::to_string(new_setting.type_id), sqlite3pp::nocopy);
	cmd.bind(":channel", std::to_string(new_setting.channel_id), sqlite3pp::nocopy);
	cmd.bind(":severity", std::to_string(static_cast<int>(new_setting.severity)), sqlite3pp::nocopy);
	cmd.bind(":threshold_type", std::to_string(static_cast<int>(new_setting.threshold)), sqlite3pp::nocopy);
	cmd.bind(":fixed_threshold", std::to_string(new_setting.customLevel ? static_cast<int>(*new_setting.customLevel) : 0), sqlite3pp::nocopy);

	return cmd.execute();
}

std::vector<alarm_level_history_point> Database::get_alarm_level_history(const alarm_settings_t& associated_alarm) {
	std::lock_guard guard(database_access_mutex);

	const auto statement = "SELECT level, time_since_epoch FROM alarm_levels WHERE channel_id = " + std::to_string(associated_alarm.channel_id) +
						   " AND type_id = " + std::to_string(associated_alarm.type_id) + " AND alarm_severity = " + std::to_string(static_cast<int>(associated_alarm.severity));

	return get_query_results<alarm_level_history_point>(statement.c_str(), database, [](const auto row)
	{
		const auto[level, seconds_since_epoch] = row.get_columns<double, int>(0, 1);

		return alarm_level_history_point(time_point_t(seconds(seconds_since_epoch)), level);
	});
}

void Database::add_alarm_level_history_item(const time_point_t& occurence, const alarm_settings_t& associated_alarm, double new_level) {
	std::lock_guard guard(database_access_mutex);

	sqlite3pp::command cmd(*database, add_alarm_level_history_point);

	cmd.bind(":type", associated_alarm.type_id);
	cmd.bind(":channel", associated_alarm.channel_id);
	cmd.bind(":severity", static_cast<int>(associated_alarm.severity));
	cmd.bind(":level", new_level);
	cmd.bind(":time_since_epoch", seconds_since_epoch(occurence));

	cmd.execute();
}

Database::Database(std::shared_ptr<IDatabaseFactory> db_factory) : factory(std::move(db_factory)) {
	database = DatabaseInitialiser::intialise_database(factory->get_database());
}
