#include <gtest/gtest.h>
#include <Hypodermic/Hypodermic.h>
#include <range/v3/all.hpp>
#include <Database.h>
#include <array>
#include <DatabaseFactory.h>
#include <SQLiteCpp/Transaction.h>
#include <DatabaseInitialiser.h>
#include <ConfigStorage.h>

using namespace std::string_view_literals;
using namespace ranges;

class ConfigStorageTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::shared_ptr<IDatabaseFactory> factory = std::make_shared<DatabaseFactory>();
        raw_database = factory->get_database(":memory:");
        raw_database = DatabaseInitialiser::intialise_database(std::move(raw_database));
        config_storage = std::make_shared<ConfigStorage>(factory);
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
    std::shared_ptr<IConfigurationAccess> config_storage;
};

TEST_F(ConfigStorageTest, Constructs) {
    EXPECT_TRUE(true);
}


TEST_F(ConfigStorageTest, CanRetrieveSiteInformation) {
    execute_setup_statements<2>({R"(INSERT INTO sites (site_name) VALUES ("Highbury"))", R"(INSERT INTO sites (site_name) VALUES ("Bath"))"});

    const auto sites = config_storage->get_site_data();

    ASSERT_EQ(2, sites.size());
    EXPECT_EQ(1, sites.at(0).id);
    EXPECT_EQ("Highbury", sites.at(0).name);
    EXPECT_EQ(2, sites.at(1).id);
    EXPECT_EQ("Bath", sites.at(1).name);
}

TEST_F(ConfigStorageTest, CanRetrieveMachineInformation) {
    execute_setup_statements<3>({R"(INSERT INTO sites (site_name) VALUES ("Highbury"))",
                                 R"(INSERT INTO machines (machine_name, site_id) VALUES ("M1", 1))",
                                 R"(INSERT INTO machines (machine_name, site_id) VALUES ("M2", 1))"});

    const auto sites = config_storage->get_site_data();
    const auto machines = config_storage->get_machines_for_site(sites.at(0));

    ASSERT_EQ(2, machines.size());
    EXPECT_EQ(1, machines.at(0).id);
    EXPECT_EQ("M1", machines.at(0).name);
    EXPECT_EQ(2, machines.at(1).id);
    EXPECT_EQ("M2", machines.at(1).name);
}

TEST_F(ConfigStorageTest, CanRetrieveChannelInformation) {
    execute_setup_statements<4>({R"(INSERT INTO sites (site_name) VALUES ("Highbury"))",
                                 R"(INSERT INTO machines (machine_name, site_id) VALUES ("M1", 1))",
                                 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C1", "g", 1))",
                                 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C2", "mm/s2", 1))"});

    auto test_machine = machine(1);
    const auto returned_channels = config_storage->get_channel_information_for_machine(test_machine);

    ASSERT_EQ(2, returned_channels.size());

    EXPECT_EQ(1, returned_channels.at(0).id);
    EXPECT_EQ("C1", returned_channels.at(0).name);
    EXPECT_EQ("g", returned_channels.at(0).units);
    EXPECT_EQ(2, returned_channels.at(1).id);
    EXPECT_EQ("C2", returned_channels.at(1).name);
    EXPECT_EQ("mm/s2", returned_channels.at(1).units);
}

TEST_F(ConfigStorageTest, CanGetMachineIDFromChannelID) {
    execute_setup_statements<4>({R"(INSERT INTO machines (machine_name, site_id) VALUES ("M1", 1))",
                                 R"(INSERT INTO machines (machine_name, site_id) VALUES ("M2", 1))",
                                 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C1", "g", 2))",
                                 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C2", "g", 1))"});

    EXPECT_EQ(1, config_storage->get_machine_id_from_channel_id(2));
    EXPECT_EQ(2, config_storage->get_machine_id_from_channel_id(1));
}

TEST_F(ConfigStorageTest, CanGetAllChannels) {
    execute_setup_statements<14>({R"(INSERT INTO sites (site_name) VALUES ("Highbury"))",
                                 R"(INSERT INTO sites (site_name) VALUES ("Manderley"))",
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
                                 R"(INSERT INTO channels (channel_name, channel_units, machine_id) VALUES ("C8", "g", 4))"});

    const auto channels = config_storage->get_all_channels();
    ASSERT_EQ(8, channels.size());

    for (auto channel_number : ranges::views::iota(1, 9)) {
        const auto found = find(channels, channel_number, &channel::id) != ranges::end(channels);
        const auto correct_name = find(channels, "C" + std::to_string(channel_number), &channel::name) != ranges::end(channels);
        ASSERT_TRUE(found && correct_name);
    }
}
