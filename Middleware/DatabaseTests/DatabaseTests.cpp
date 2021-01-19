#include <gtest/gtest.h>
#include <Hypodermic/Hypodermic.h>
#include <Database.h>
#include "MockDatabaseFactory.h"

class DatabaseTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::shared_ptr<IDatabaseFactory> factory = std::make_shared<MockDatabaseFactory>();
		database = std::make_shared<Database>(factory);
		raw_database = factory->get_database();
	}

	void TearDown() override {
	}

	std::shared_ptr<sqlite3pp::database> raw_database;
	std::shared_ptr<IDatabase> database;
};

TEST_F(DatabaseTest, Constructs) {
	EXPECT_TRUE(true);
}

TEST_F(DatabaseTest, CanRetrieveSiteInformation) {
	sqlite3pp::command insert_site1(*raw_database, "INSERT INTO sites (site_name) VALUES (\"Highbury\")");
	insert_site1.execute();
	sqlite3pp::command insert_site2(*raw_database, "INSERT INTO sites (site_name) VALUES (\"Bath\")");
	insert_site2.execute();

	const auto sites = database->get_site_data();

	ASSERT_EQ(2, sites.size());
	EXPECT_EQ(1, sites.at(0).id);
	EXPECT_EQ("Highbury", sites.at(0).name);
	EXPECT_EQ(2, sites.at(1).id);
	EXPECT_EQ("Bath", sites.at(1).name);
}