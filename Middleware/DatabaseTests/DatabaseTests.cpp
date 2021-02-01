#include <gtest/gtest.h>
#include <Hypodermic/Hypodermic.h>
#include <range/v3/all.hpp>
#include <Database.h>
#include <array>
#include <DatabaseFactory.h>
#include <SQLiteCpp/Transaction.h>
#include <DatabaseInitialiser.h>

using namespace std::string_view_literals;
using namespace ranges;

class DatabaseTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::shared_ptr<IDatabaseFactory> factory = std::make_shared<DatabaseFactory>();
        raw_database = factory->get_database(":memory:");
		config_storage = std::make_shared<Database>(factory);
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
	std::shared_ptr<IDatabase> config_storage;
};

TEST_F(DatabaseTest, Constructs) {
	EXPECT_TRUE(true);
}

TEST_F(DatabaseTest, CanRetrieveData) {
	// This exploits a bug, whereby the data table doesn't have a proper foreign key into the channels or types, which it should, meaining less set up is required.
	execute_setup_statements<4>({R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 2, 123, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 1, 123, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 124, 45.7))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 123, 45.6))"});

	const auto data = config_storage->get_data(2, 2, time_point_t(0s), time_point_t(100000s));

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

	const auto data = config_storage->get_data(2, 2, time_point_t(101s), time_point_t(104s));

	EXPECT_EQ(4, data.size()); // Should filter out the points on the wrong channel, and for the wrong type, respectively.
}

TEST_F(DatabaseTest, CanAddAlarmLevelInformation) {
	const auto alarm_settings = alarm_settings_t(1, 1, alarmSeverity::alarm, alarmThreshold::automaticBasic);

	config_storage->add_alarm_level_history_item(time_point_t(100s), alarm_settings, 123.0);
	config_storage->add_alarm_level_history_item(time_point_t(200s), alarm_settings, 456.0);

	const auto retrieved_history = config_storage->get_alarm_level_history(alarm_settings);

	ASSERT_EQ(2, retrieved_history.size());

	EXPECT_EQ(time_point_t(100s), retrieved_history.at(0).time_changed);
	EXPECT_EQ(123.0, retrieved_history.at(0).level);
	EXPECT_EQ(time_point_t(200s), retrieved_history.at(1).time_changed);
	EXPECT_EQ(456.0, retrieved_history.at(1).level);
}

TEST_F(DatabaseTest, CanAddAlarmFiringInformation) {
	execute_setup_statements<1>({R"(INSERT INTO channels (channel_units, channel_name, machine_id) VALUES ("g", "C1", 1))"});

	config_storage->add_alarm_activation(alarm_activation_t(1, 1, alarmSeverity::alert, time_point_t(100s), false));
	config_storage->add_alarm_activation(alarm_activation_t(1, 1, alarmSeverity::alarm, time_point_t(110s), true));
	config_storage->add_alarm_activation(alarm_activation_t(1, 1, alarmSeverity::alert, time_point_t(120s), false));

	const auto retrieved_history = config_storage->get_activations_for_machine(1); // Need to link the channel to the machine somehow

	ASSERT_EQ(3, retrieved_history.size());

	EXPECT_EQ(time_point_t(100s), retrieved_history.at(0).activation_time);
	EXPECT_EQ(alarmSeverity::alert, retrieved_history.at(0).severity);

	EXPECT_EQ(time_point_t(110s), retrieved_history.at(1).activation_time);
	EXPECT_EQ(alarmSeverity::alarm, retrieved_history.at(1).severity);

	EXPECT_EQ(time_point_t(120s), retrieved_history.at(2).activation_time);
	EXPECT_EQ(alarmSeverity::alert, retrieved_history.at(2).severity);
}

TEST_F(DatabaseTest, CanFindAllTypesWithDataOnGivenChannel) {
	// This exploits a bug, whereby the data table doesn't have a proper foreign key into the channels or types, which it should, meaning less set up is required.
	execute_setup_statements<9>({R"(INSERT INTO sites (site_name) VALUES ("Highbury"))",
								 R"(INSERT INTO machines (machine_name, site_id) VALUES ("M1", 1))",
								 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C1", "g", 1))",
								 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C2", "g", 1))",
								 R"(INSERT INTO types (type_name) VALUES ("T1"))", // Type has data and is on the correct channel
								 R"(INSERT INTO types (type_name) VALUES ("T2"))", // Type has no data but is on the same channel
								 R"(INSERT INTO types (type_name) VALUES ("T3"))", // Type has data but is on a different channel
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 1, 123, 45.5))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (3, 2, 123, 45.5))"});

	const auto results = config_storage->get_data_types_available_for_channel(1);

	ASSERT_EQ(1, results.size());
	EXPECT_EQ(1, results.at(0).first);
	EXPECT_EQ("T1", results.at(0).second);
}

TEST_F(DatabaseTest, CanGetEarliestDataPointForMachine) {
	execute_setup_statements<7>({R"(INSERT INTO machines (machine_name, site_id) VALUES ("M1", 1))",
								 R"(INSERT INTO machines (machine_name, site_id) VALUES ("M2", 1))",
								 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C1", "g", 1))",
								 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C2", "g", 2))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 1, 123, 100))", // Earliest point, but on machine 1
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 2, 124, 200))", // Earliest on machine 2
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 2, 126, 300))"});

	EXPECT_EQ(time_point_t(124s), config_storage->get_earliest_data_point_for_machine(2));
}


TEST_F(DatabaseTest, CanRetrieveAlarmSettings) {
	execute_setup_statements<10>({R"(INSERT INTO machines (machine_name, site_id) VALUES ("M1", 1))",
								  R"(INSERT INTO machines (machine_name, site_id) VALUES ("M2", 1))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C1", "g", 1))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C2", "g", 2))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C3", "g", 2))",
								  R"(INSERT INTO types (type_name) VALUES ("T1"))",
								  R"(INSERT INTO types (type_name) VALUES ("T2"))"
								  R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 1, 123, 100))",
								  R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 2, 124, 200))",
								  R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 126, 300))"});

	raw_database = DatabaseInitialiser::intialise_database(std::move(raw_database)); // Re-initialise so it can generate the default alarm settings

	const auto retrieved_alarm_settings = config_storage->get_alarm_settings_for_machine(2);
	ASSERT_EQ(8, retrieved_alarm_settings.size());
}

TEST_F(DatabaseTest, CanUpdateAlarmSettings) {
	execute_setup_statements<10>({R"(INSERT INTO machines (machine_name, site_id) VALUES ("M1", 1))",
								  R"(INSERT INTO machines (machine_name, site_id) VALUES ("M2", 1))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C1", "g", 1))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C2", "g", 2))",
								  R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C3", "g", 2))",
								  R"(INSERT INTO types (type_name) VALUES ("T1"))",
								  R"(INSERT INTO types (type_name) VALUES ("T2"))"
								  R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 1, 123, 100))",
								  R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 2, 124, 200))",
								  R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 126, 300))"});

	raw_database = DatabaseInitialiser::intialise_database(std::move(raw_database)); // Re-initialise so it can generate the default alarm settings

	const auto x = config_storage->update_alarm_setting(alarm_settings_t(2, 2, alarmSeverity::alarm, alarmThreshold::Custom, 500.f));
	const auto retrieved_alarm_settings = config_storage->get_alarm_settings_for_machine(2);
	const auto changed_setting = find_if(retrieved_alarm_settings, [](const auto& settings) { return settings.channel_id == 2 && settings.type_id == 2 && settings.severity == alarmSeverity::alarm; });
	ASSERT_NE(changed_setting, ranges::end(retrieved_alarm_settings));

	EXPECT_EQ(changed_setting->threshold, alarmThreshold::Custom);
	EXPECT_TRUE(changed_setting->customLevel.has_value());
	EXPECT_EQ(500.f, changed_setting->customLevel.value());
}
