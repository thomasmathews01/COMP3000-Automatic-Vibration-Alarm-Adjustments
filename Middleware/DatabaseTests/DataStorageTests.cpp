#include <gtest/gtest.h>
#include <Hypodermic/Hypodermic.h>
#include <range/v3/all.hpp>
#include <Database.h>
#include <array>
#include <DatabaseFactory.h>
#include <SQLiteCpp/Transaction.h>
#include <DatabaseInitialiser.h>
#include <DataStorage.h>
#include "Mocks/AllMocks.h"

using namespace std::string_view_literals;
using namespace ranges;

class DataStorageTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::shared_ptr<IDatabaseFactory> factory = std::make_shared<DatabaseFactory>();
        raw_database = factory->get_database(":memory:");
        raw_database = DatabaseInitialiser::intialise_database(std::move(raw_database));
        auto clock = std::make_shared<MockClock>();
        data_storage = std::make_shared<DataStorage>(factory, clock);
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
    std::shared_ptr<IDataAccess> data_storage;
};

TEST_F(DataStorageTest, Constructs) {
    EXPECT_TRUE(true);
}

TEST_F(DataStorageTest, CanFindAllTypesWithDataOnGivenChannel) {
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

	const auto results = data_storage->get_data_types_available_for_channel(1);

	ASSERT_EQ(1, results.size());
	EXPECT_EQ(1, results.at(0).first);
	EXPECT_EQ("T1", results.at(0).second);
}

TEST_F(DataStorageTest, CanGetEarliestDataPointForMachine) {
	execute_setup_statements<7>({R"(INSERT INTO machines (machine_name, site_id) VALUES ("M1", 1))",
								 R"(INSERT INTO machines (machine_name, site_id) VALUES ("M2", 1))",
								 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C1", "g", 1))",
								 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C2", "g", 2))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 1, 123, 100))", // Earliest point, but on machine 1
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 2, 124, 200))", // Earliest on machine 2
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 2, 126, 300))"});

	EXPECT_EQ(time_point_t(124s), data_storage->get_earliest_data_point_for_machine(2));
}

TEST_F(DataStorageTest, CanRetrieveData) {
	// This exploits a bug, whereby the data table doesn't have a proper foreign key into the channels or types, which it should, meaining less set up is required.
	execute_setup_statements<4>({R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (1, 2, 123, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 1, 123, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 124, 45.7))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 123, 45.6))"});

	const auto data = data_storage->get_data(2, 2, time_point_t(0s), time_point_t(100000s));

	ASSERT_EQ(2, data.size()); // Should filter out the points on the wrong channel, and for the wrong type, respectively.

	EXPECT_EQ(time_point_t(123s), data.at(0).first);
	EXPECT_EQ(45.6f, data.at(0).second);
	EXPECT_EQ(time_point_t(124s), data.at(1).first);
	EXPECT_EQ(45.7f, data.at(1).second);
}

TEST_F(DataStorageTest, CanRetrieveDataWithinTimeRangeInclusiveOfStartAndFinish) {
	// This exploits a bug, whereby the data table doesn't have a proper foreign key into the channels or types, which it should, meaning less set up is required.
	execute_setup_statements<6>({R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 100, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 101, 45.6))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 102, 45.7))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 103, 45.8))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 104, 45.9))",
								 R"(INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES (2, 2, 105, 46.0))"});

	const auto data = data_storage->get_data(2, 2, time_point_t(101s), time_point_t(104s));

	EXPECT_EQ(4, data.size()); // Should filter out the points on the wrong channel, and for the wrong type, respectively.
}
