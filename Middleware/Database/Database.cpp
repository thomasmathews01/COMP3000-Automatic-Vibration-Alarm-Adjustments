#include "Database.h"
#include "doctest/doctest.h"

using namespace std::string_literals;

std::vector<site> Database::get_site_data() {
	std::lock_guard guard(database_access_mutex);

	std::vector<site> sites;

	for (auto row : sqlite3pp::query(database, "SELECT site_id, site_name from SITES")) {
		const auto[id_number, name_string] = row.get_columns<int, const char*>(0, 1);
		sites.emplace_back(id_number, name_string);
	}

	return sites;
}

bool site_contains_machine_with_id(const site& site, int id) {
	return std::find_if(site.machines.cbegin(), site.machines.cend(), [id](const auto& machine) { return id == machine.id; }) != site.machines.cend();
}

void Database::populate_sites_machine_information(site& site) {
	std::lock_guard guard(database_access_mutex);

	const auto query_string = "SELECT machine_id, machine_name from machines WHERE site_id = "s + std::to_string(site.id);

	for (auto row : sqlite3pp::query(database, query_string.c_str())) {
		const auto[machine_id, machine_name] = row.get_columns<int, const char*>(0, 1);

		if (!site_contains_machine_with_id(site, machine_id))
			site.machines.emplace_back(machine_id, machine_name);
	}
}

bool machine_contains_channel_with_id(const machine& machine, int channel_id) {
	return std::find_if(machine.channels.cbegin(), machine.channels.cend(), [channel_id](const auto& channel) { return channel.id == channel_id; }) != machine.channels.cend();
}

void Database::populate_channel_information_for_a_machine(machine& machine) {
	std::lock_guard guard(database_access_mutex);

	const auto channel_query_string = "SELECT channel_id, channel_name, channel_units FROM channels WHERE machine_id = "s + std::to_string(machine.id);

	for (auto row : sqlite3pp::query(database, channel_query_string.c_str())) {
		const auto[id, name, units] = row.get_columns<int, const char*, const char*>(0, 1, 2);

		if (!machine_contains_channel_with_id(machine, id))
			machine.channels.emplace_back(id, name, units);
	}
}

void Database::set_up_database_connection() {
	std::lock_guard<std::mutex> guard(database_access_mutex);
	database = sqlite3pp::database(default_database_location);
}

std::vector<std::pair<time_point_t, float>> Database::get_data(int channel, int type, time_point_t start, time_point_t end) {
	return std::vector<std::pair<time_point_t, float>>();
}

std::vector<std::pair<int, std::string>> Database::get_data_types_available_for_channel(int channel_id) {
	return std::vector<std::pair<int, std::string>>();
}

int Database::get_machine_id_from_channel_id(int channel_id) {
	std::lock_guard guard(database_access_mutex);

	const auto selection_string = "SELECT machine_id from channels where channel_id = " + std::to_string(channel_id);

	for (const auto& row : sqlite3pp::query(database, selection_string.c_str())) {
		const auto[machine_id] = row.get_columns<int>(0);
		return machine_id;
	}

	return -1;
}

time_point_t Database::get_earliest_data_point_for_machine(int machine_id) {
	return time_point_t();
}

std::vector<state_change_t> Database::get_state_changes_for_machine(int machine_id) {
	return std::vector<state_change_t>();
}

TEST_CASE ("Doesn't throw when getting site data from database") {
	Database db;
	db.set_up_database_connection(); // TODO: This should be called from the constructor, or otherwise dealt with in a way that makes more sense than this two phase initialisation.

	const auto sites = db.get_site_data();

		CHECK_EQ(1, sites.size());
}

TEST_CASE ("Retrieves accurate site information from database") {
	Database db;
	db.set_up_database_connection();
	const auto expected_name = "Hunterston B"s;

	const auto sites = db.get_site_data();

		CHECK_EQ(sites.at(0).id, 1);
		CHECK(std::equal(sites.at(0).name.cbegin(), sites.at(0).name.cend(), expected_name.cbegin()));
}

TEST_CASE ("Retrieves machine information for a given site") {
	Database db;
	db.set_up_database_connection();
	const auto expected_name = "Hunterston B"s;

	const auto sites = db.get_site_data();

		CHECK_EQ(sites.at(0).id, 1);
		CHECK(std::equal(sites.at(0).name.cbegin(), sites.at(0).name.cend(), expected_name.cbegin()));
}

TEST_CASE ("All machines extracted for a site") {
	Database db;
	db.set_up_database_connection();

	site test_site(1, "Hunterston B");
	db.populate_sites_machine_information(test_site);

		CHECK_EQ(24, test_site.machines.size());
}

TEST_CASE ("Machine names extracted for a site") {
	Database db;
	db.set_up_database_connection();

	site test_site(1, "Hunterston B");
	db.populate_sites_machine_information(test_site);

		CHECK(std::find_if(test_site.machines.cbegin(), test_site.machines.cend(), [](const machine& machine) { return machine.name == "TG 7"s; }) != test_site.machines.cend());
		CHECK(std::find_if(test_site.machines.cbegin(), test_site.machines.cend(), [](const machine& machine) { return machine.name == "TG 8"s; }) != test_site.machines.cend());
}

TEST_CASE ("All channels extracted for a site") {
	Database db;
	db.set_up_database_connection();

	machine test_machine(21, "Hunterston B");
	db.populate_channel_information_for_a_machine(test_machine);

		CHECK_EQ(30, test_machine.channels.size());
}