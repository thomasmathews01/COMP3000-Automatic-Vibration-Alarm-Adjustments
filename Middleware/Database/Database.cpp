#include "Database.h"
#include "doctest/doctest.h"
#include <filesystem>

constexpr auto create_table_statement = "CREATE TABLE sites ("
										"site_id INT NOT NULL PRIMARY KEY,"
										"site_name varchar(255)"
										")";

constexpr auto create_machines_table_statement = "CREATE TABLE machines ("
												 "machine_id INT NOT NULL PRIMARY KEY,"
												 "machine_name varchar(255)"
												 ")";

constexpr auto create_machine_allocations_table = "CREATE TABLE allocs ("
												  "ID INT NOT NULL PRIMARY KEY"
												  "site_id INT NOT NULL"
												  "machine_id INT NOT NULL"
												  "FOREIGN KEY (site_id) REFERENCES sites (site_id)"
												  "FOREIGN KEY (machine_id) REFERENCES sites (machine_id)";

std::vector<site> Database::get_site_data() {
	std::lock_guard<std::mutex> guard(database_access_mutex);

	return std::vector<site>();
}

void Database::set_up_database() {
	std::lock_guard<std::mutex> guard(database_access_mutex);
	database = sqlite3pp::database("VibrationData.db");

	database.execute(create_table_statement);
	database.execute(create_machines_table_statement);
}

void Database::add_site_with_machines_to_database(const site& s) {
	constexpr auto insert_site_statement = "INSERT INTO SITES (site_id, site_name) VALUES (1,\"Site 1\"";
	database.execute(insert_site_statement);
}

TEST_CASE ("Can instantiate a database") {
	auto database = Database();
	database.set_up_database();

		CHECK(std::filesystem::exists("VibrationData.db"));
}

TEST_CASE ("Can instantiate a database") {
	auto database = Database();
	database.set_up_database();

		CHECK(std::filesystem::exists("VibrationData.db"));
}