#include <gtest/gtest.h>
#include <Hypodermic/Hypodermic.h>
#include <Database.h>
#include <array>
#include "MockDatabaseFactory.h"
#include <SQLiteCpp/Transaction.h>
#include <DatabaseInitialiser.h>
#include <database_statements.h>

using namespace std::string_view_literals;

class DatabaseInitialiserTest : public ::testing::Test {
protected:
	void SetUp() override {
		auto factory = std::make_shared<MockDatabaseFactory>();
		database = DatabaseInitialiser::intialise_database(factory->get_database()); // Set up all the required tables.
	}

	void TearDown() override {
	}

	template<int N>
	void execute_setup_statements(const std::array<std::string_view, N>& statements) {
		sqlite3pp::transaction xct(*database);

		for (const auto statement : statements)
			database->execute(statement.data());

		xct.commit();
	}

	std::shared_ptr<sqlite3pp::database> database;
};

TEST_F(DatabaseInitialiserTest, CanInsertAlarmSettings) {
	database = DatabaseInitialiser::intialise_database(std::move(database)); // Should now fill the tables with defaults.

	sqlite3pp::command cmd(*database, add_alarm_settings);

	cmd.bind(":type", "1", sqlite3pp::nocopy);
	cmd.bind(":channel", "1", sqlite3pp::nocopy);
	cmd.bind(":severity", "1", sqlite3pp::nocopy);
	cmd.bind(":threshold_type", "1", sqlite3pp::nocopy);
	cmd.bind(":custom_threshold", "1", sqlite3pp::nocopy);

	cmd.execute();

	auto query = sqlite3pp::query(*database, "SELECT COUNT(*) from alarm_settings");
	auto row = query.begin();
	const auto[alarm_settings_count] = (*row).get_columns<int>(0);

	EXPECT_EQ(1, alarm_settings_count);
}

TEST_F(DatabaseInitialiserTest, CreatesAlarmSettingsForAllChannelsOnStartup) {
	// This exploits a bug, whereby the data table doesn't have a proper foreign key into the channels or types, which it should, meaning less set up is required.
	execute_setup_statements<18>({R"(INSERT INTO sites (site_name) VALUES ("Highbury"))",
								  R"(INSERT INTO sites (site_name) VALUES ("Bath"))",
								  R"(INSERT INTO machines (machine_name, site_id) VALUES ("M1", 1))",
								  R"(INSERT INTO machines (machine_name, site_id) VALUES ("M2", 1))",
								  R"(INSERT INTO machines (machine_name, site_id) VALUES ("M3", 2))",
								  R"(INSERT INTO machines (machine_name, site_id) VALUES ("M4", 2))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C1", "g", 1))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C2", "g", 1))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C3", "g", 2))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C4", "g", 2))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C5", "g", 3))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C6", "g", 3))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C7", "g", 4))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C8", "g", 4))",
								  R"(INSERT INTO types (type_name) VALUES ("T1"))",
								  R"(INSERT INTO types (type_name) VALUES ("T1"))",
								  R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 1, 123, 45.5))",
								  R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 1, 123, 45.5))"});

	database = DatabaseInitialiser::intialise_database(std::move(database)); // Should now fill the tables with defaults.

	const auto[alarm_settings_count] = (*sqlite3pp::query(*database, "SELECT COUNT(*) from alarm_settings").begin()).get_columns<int>(0);

	EXPECT_EQ(32, alarm_settings_count); // 8 unique channels, with 2 data types, 2 severities (alert & alarm) should generate 32 alarms.
}