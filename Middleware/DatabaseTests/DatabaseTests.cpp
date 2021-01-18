#include <gtest/gtest.h>
#include <Hypodermic/Hypodermic.h>
#include <Database.h>
#include "MockDatabaseFactory.h"

class DatabaseTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::shared_ptr<IDatabaseFactory> factory = std::make_shared<MockDatabaseFactory>();
		database = std::make_shared<Database>(factory);
	}

	void TearDown() override {
	}

	std::shared_ptr<IDatabase> database;
};

TEST_F(DatabaseTest, Constructs) {
	EXPECT_TRUE(true);
}