/*
#include <gtest/gtest.h>
#include <range/v3/all.hpp>
#include <array>
#include <filesystem>
#include <DatabaseFactory.h>
#include <AlarmStorage.h>
#include <DataStorage.h>
#include <ConfigStorage.h>
#include <Hypodermic/Hypodermic.h>

using namespace std::string_view_literals;
using namespace ranges;

class MachineLearningTest : public ::testing::Test {
protected:
	void SetUp() override {
		auto memory_database = std::make_shared<sqlite3pp::database>(sqlite3pp::database(":memory:"));
		auto real_database = std::make_shared<sqlite3pp::database>("/Users/thomasmathews/Downloads/VibrationData.db");

		const auto start = std::chrono::high_resolution_clock::now();
		real_database->backup(*memory_database);
		const auto end = std::chrono::high_resolution_clock::now();

		const auto backup_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "Backup: " << backup_time_ms <<std::endl;

	 	 const auto factory = std::make_shared<DatabaseFactory>(std::move(memory_database));
	 	 data_storage = std::make_shared<DataStorage>(factory);
	 	 config_storage = std::make_shared<ConfigStorage>(factory);
	}

	void TearDown() override {

	}

	std::shared_ptr<IDataAccess> data_storage;
	std::shared_ptr<IConfigurationAccess> config_storage;
};

TEST_F(MachineLearningTest, CanAddAlarmLevelInformation) {
 	const auto start = std::chrono::high_resolution_clock::now();
	const auto get_data = data_storage->get_data_types_available_for_channel(161);
	const auto end = std::chrono::high_resolution_clock::now();

	const auto fetch_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Query: " << fetch_time_ms << std::endl; // Cut the time by half, but for 26 channels this is still  2 minutes, just for the data types.


	FAIL();
}

TEST_F(MachineLearningTest, GetDimensions) {
 	const machine test_machine(21);
 	const auto channels = config_storage->get_channel_information_for_machine(test_machine);
 	std::vector<int> types_count;

 	for (auto& channel : channels)
 		types_count.push_back(data_storage->get_data_types_available_for_channel(channel.id).size());

 	const auto final_types_required = ranges::accumulate(types_count, 0);
 	FAIL();
}
*/
