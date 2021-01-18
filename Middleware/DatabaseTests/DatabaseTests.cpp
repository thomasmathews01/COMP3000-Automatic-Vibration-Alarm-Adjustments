#include <gtest/gtest.h>
#include <Hypodermic/Hypodermic.h>
#include <Database.h>
#include "MockDatabaseFactory.h"

class DatabaseTest : public ::testing::Test {
protected:
	void SetUp() override {
		Hypodermic::ContainerBuilder builder;

		builder.registerType<MockDatabaseFactory>().as<IDatabaseFactory>().singleInstance();

		container = builder.build();
	}

	void TearDown() override {
	}

	std::shared_ptr<Hypodermic::Container> container;
};

TEST_F(DatabaseTest, Constructs) {
	auto database = container->resolve<IDatabase>();
	EXPECT_TRUE(true);
}