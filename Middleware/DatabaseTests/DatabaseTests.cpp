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