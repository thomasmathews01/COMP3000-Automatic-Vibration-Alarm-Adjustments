#include <gtest/gtest.h>
#include <Hypodermic/Hypodermic.h>
#include <Database.h>
#include <array>
#include "MockDatabaseFactory.h"
#include <SQLiteCpp/Transaction.h>

using namespace std::string_view_literals;

class DatabaseTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::shared_ptr<IDatabaseFactory> factory = std::make_shared<MockDatabaseFactory>();
		database = std::make_shared<Database>(factory);
		raw_database = factory->get_database();
	}

	void TearDown() override {
	}

	template<int N>
	void execute_setup_statements(const std::array<std::string_view, N>& statements) {
		sqlite3pp::transaction xct(*raw_database);

		for (const auto statement : statements)
			raw_database->execute(statement.data());

		xct.commit();
	}

	std::shared_ptr<sqlite3pp::database> raw_database;
	std::shared_ptr<IDatabase> database;
};

TEST_F(DatabaseTest, Constructs) {
	EXPECT_TRUE(true);
}

TEST_F(DatabaseTest, CanRetrieveSiteInformation) {
	execute_setup_statements<2>({R"(INSERT INTO sites (site_name) VALUES ("Highbury"))", R"(INSERT INTO sites (site_name) VALUES ("Bath"))"});

	const auto sites = database->get_site_data();

	ASSERT_EQ(2, sites.size());
	EXPECT_EQ(1, sites.at(0).id);
	EXPECT_EQ("Highbury", sites.at(0).name);
	EXPECT_EQ(2, sites.at(1).id);
	EXPECT_EQ("Bath", sites.at(1).name);
}

TEST_F(DatabaseTest, CanRetrieveMachineInformation) {
	execute_setup_statements<3>({R"(INSERT INTO sites (site_name) VALUES ("Highbury"))",
								 R"(INSERT INTO machines (machine_name, site_id) VALUES ("M1", 1))",
								 R"(INSERT INTO machines (machine_name, site_id) VALUES ("M2", 1))"});

	const auto sites = database->get_site_data();
	const auto machines = database->get_machines_for_site(sites.at(0));

	ASSERT_EQ(2, machines.size());
	EXPECT_EQ(1, machines.at(0).id);
	EXPECT_EQ("M1", machines.at(0).name);
	EXPECT_EQ(2, machines.at(1).id);
	EXPECT_EQ("M2", machines.at(1).name);
}

TEST_F(DatabaseTest, CanRetrieveChannelInformation) {
	execute_setup_statements<4>({R"(INSERT INTO sites (site_name) VALUES ("Highbury"))",
								 R"(INSERT INTO machines (machine_name, site_id) VALUES ("M1", 1))",
								 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C1", "g", 1))",
								 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C2", "mm/s2", 1))"});

	auto test_machine = machine(1);
	const auto returned_channels = database->get_channel_information_for_machine(test_machine);

	ASSERT_EQ(2, returned_channels.size());

	EXPECT_EQ(1, returned_channels.at(0).id);
	EXPECT_EQ("C1", returned_channels.at(0).name);
	EXPECT_EQ("g", returned_channels.at(0).units);
	EXPECT_EQ(2, returned_channels.at(1).id);
	EXPECT_EQ("C2", returned_channels.at(1).name);
	EXPECT_EQ("mm/s2", returned_channels.at(1).units);
}

TEST_F(DatabaseTest, CanRetrieveData) {
	// This exploits a bug, whereby the data table doesn't have a proper foreign key into the channels or types, which it should, meaining less set up is required.
	execute_setup_statements<4>({R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 2, 123, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 1, 123, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 124, 45.7))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 123, 45.6))"});

	const auto data = database->get_data(2, 2, time_point_t(0s), time_point_t(100000s));

	ASSERT_EQ(2, data.size()); // Should filter out the points on the wrong channel, and for the wrong type, respectively.

	EXPECT_EQ(time_point_t(123s), data.at(0).first);
	EXPECT_EQ(45.6f, data.at(0).second);
	EXPECT_EQ(time_point_t(124s), data.at(1).first);
	EXPECT_EQ(45.7f, data.at(1).second);
}

TEST_F(DatabaseTest, CanRetrieveDataWithinTimeRangeInclusiveOfStartAndFinish) {
	// This exploits a bug, whereby the data table doesn't have a proper foreign key into the channels or types, which it should, meaning less set up is required.
	execute_setup_statements<6>({R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 100, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 101, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 102, 45.7))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 103, 45.8))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 104, 45.9))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 105, 46.0))"});

	const auto data = database->get_data(2, 2, time_point_t(101s), time_point_t(104s));

	EXPECT_EQ(4, data.size()); // Should filter out the points on the wrong channel, and for the wrong type, respectively.
}

TEST_F(DatabaseTest, CanAddAlarmLevelInformation) {
	// This exploits a bug, whereby the data table doesn't have a proper foreign key into the channels or types, which it should, meaning less set up is required.
	execute_setup_statements<6>({R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 100, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 101, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 102, 45.7))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 103, 45.8))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 104, 45.9))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 105, 46.0))"});

	const auto alarm_settings = alarm_settings_t(1, 1, alarmSeverity::alarm, alarmThreshold::automaticBasic);

	database->add_alarm_level_history_item(time_point_t(100s), alarm_settings, 123.0);
	database->add_alarm_level_history_item(time_point_t(200s), alarm_settings, 456.0);

	const auto retrieved_history = database->get_alarm_level_history(alarm_settings);

	ASSERT_EQ(2, retrieved_history.size());

	EXPECT_EQ(time_point_t(100s), retrieved_history.at(0).time_changed);
	EXPECT_EQ(123.0, retrieved_history.at(0).level);
	EXPECT_EQ(time_point_t(200s), retrieved_history.at(1).time_changed);
	EXPECT_EQ(456.0, retrieved_history.at(1).level);
}
