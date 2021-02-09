#include <gtest/gtest.h>
#include <Statistics/MLDataAggregator.h>
#include <random>
#include <range/v3/all.hpp>
#include "Mocks/AllMocks.h"

class MLDataAggregatorTests : public ::testing::Test {
protected:
	void SetUp() override {
		config_storage = std::make_shared<MockConfigurationAccess>();
		data_storage = std::make_shared<MockDataAccess>();
	}

	void TearDown() override {
	}

	std::shared_ptr<IConfigurationAccess> config_storage;
	std::shared_ptr<IDataAccess> data_storage;
};

TEST_F(MLDataAggregatorTests, canConstruct) {
	ASSERT_NO_THROW(MLDataAggregator<1>(data_storage, config_storage));
}

TEST_F(MLDataAggregatorTests, canReturnSimpleData) {
	auto mock_config_storage = std::dynamic_pointer_cast<MockConfigurationAccess>(config_storage);
	const std::vector<channel> channels = {channel{1}, channel{2}, channel{3}, channel{4}, channel{5}};
	EXPECT_CALL(*mock_config_storage, get_channel_information_for_machine(testing::A<const machine&>())).Times(1).WillRepeatedly(testing::Return(channels));

	const auto mock_data_storage = std::dynamic_pointer_cast<MockDataAccess>(data_storage);
	const auto types = ranges::views::iota(1, 6)
					   | ranges::views::transform([](const auto i) { return std::make_pair(i, std::to_string(i)); })
					   | ranges::to<std::vector>();
	EXPECT_CALL(*mock_data_storage, get_data_types_available_for_channel(testing::A<int>())).Times(5).WillRepeatedly(testing::Return(types));

	std::vector<std::pair<time_point_t, float>> first_type = {{time_point_t(1s), 5.f},
															  {time_point_t(2s), 5.f},
															  {time_point_t(3s), 5.f},
															  {time_point_t(4s), 5.f}};
	std::vector<std::pair<time_point_t, float>> other_types = {{time_point_t(1s), 6.f},
															   {time_point_t(2s), 6.f},
															   {time_point_t(3s), 6.f},
															   {time_point_t(4s), 6.f}};
	EXPECT_CALL(*mock_data_storage, get_data(testing::A<int>(), testing::A<int>(), testing::A<time_point_t>(), testing::A<time_point_t>()))
		.Times(25)
		.WillOnce(testing::Return(first_type))
		.WillRepeatedly(testing::Return(other_types));

	auto aggregator = MLDataAggregator<300>(data_storage, config_storage);
	const auto data = aggregator.get_data_before_time(1, time_point_t::max());

	ASSERT_EQ(4, data.size());
	EXPECT_EQ(time_point_t(1s), data.at(0).first);
	EXPECT_EQ(time_point_t(2s), data.at(1).first);
	EXPECT_EQ(time_point_t(3s), data.at(2).first);
	EXPECT_EQ(time_point_t(4s), data.at(3).first);

	for (const auto& arr : data | ranges::views::values) {
		for (const auto dimension : ranges::subrange(arr.cbegin(), arr.cbegin() + 1))
			EXPECT_EQ(5.f, dimension);
		for (const auto dimension : ranges::subrange(arr.cbegin() + 1, arr.cbegin() + 25))
			EXPECT_EQ(6.f, dimension);
		for (const auto dimension : ranges::subrange(arr.cbegin() + 25, arr.cend()))
			EXPECT_EQ(0.f, dimension);
	}
}
TEST_F(MLDataAggregatorTests, canHandleInterleavedData) {
	auto mock_config_storage = std::dynamic_pointer_cast<MockConfigurationAccess>(config_storage);
	const std::vector<channel> channels = {channel{1}, channel{2}, channel{3}, channel{4}, channel{5}};
	EXPECT_CALL(*mock_config_storage, get_channel_information_for_machine(testing::A<const machine&>())).Times(1).WillRepeatedly(testing::Return(channels));

	const auto mock_data_storage = std::dynamic_pointer_cast<MockDataAccess>(data_storage);
	const auto types = ranges::views::iota(1, 6)
					   | ranges::views::transform([](const auto i) { return std::make_pair(i, std::to_string(i)); })
					   | ranges::to<std::vector>();
	EXPECT_CALL(*mock_data_storage, get_data_types_available_for_channel(testing::A<int>())).Times(5).WillRepeatedly(testing::Return(types));

	std::vector<std::pair<time_point_t, float>> first_type = {{time_point_t(1s), 5.f},
															  {time_point_t(3s), 5.f},
															  {time_point_t(5s), 5.f},
															  {time_point_t(7s), 5.f}};
	std::vector<std::pair<time_point_t, float>> other_types = {{time_point_t(2s), 6.f},
															   {time_point_t(4s), 6.f},
															   {time_point_t(6s), 6.f},
															   {time_point_t(8s), 6.f}};
	EXPECT_CALL(*mock_data_storage, get_data(testing::A<int>(), testing::A<int>(), testing::A<time_point_t>(), testing::A<time_point_t>()))
		.Times(25)
		.WillOnce(testing::Return(first_type))
		.WillRepeatedly(testing::Return(other_types));

	auto aggregator = MLDataAggregator<300>(data_storage, config_storage);
	const auto data = aggregator.get_data_before_time(1, time_point_t::max());

	ASSERT_EQ(8, data.size());
	EXPECT_EQ(time_point_t(1s), data.at(0).first);
	EXPECT_EQ(time_point_t(2s), data.at(1).first);
	EXPECT_EQ(time_point_t(3s), data.at(2).first);
	EXPECT_EQ(time_point_t(4s), data.at(3).first);

	for (const auto& arr : data | ranges::views::values | ranges::views::stride(2)) {
		EXPECT_EQ(299, ranges::count(arr, 0));
		EXPECT_EQ(5.f, arr.at(0));
	}

	for (const auto& arr : data | ranges::views::values | ranges::views::drop(1) | ranges::views::stride(2)) {
		EXPECT_EQ(24, ranges::count(arr, 6.f));
		EXPECT_EQ(0.f, arr.at(0));
		EXPECT_EQ(0.f, arr.at(25));
	}
}

TEST_F(MLDataAggregatorTests, CanMergeLargeRecordSizeQuickly) {
	constexpr auto channel_count = 30;
	constexpr auto data_point_count = 1500; // 150000 is realistic for production, but this is only testable in release for obvious reasons, the optimised speedup is more than 10x.
	constexpr auto types_count = 10;

	auto mock_config_storage = std::dynamic_pointer_cast<MockConfigurationAccess>(config_storage);
	const auto channels = ranges::views::iota(1, channel_count + 1)
		 | ranges::views::transform([](const int i) { return channel{i}; })
		 | ranges::to<std::vector>();

	EXPECT_CALL(*mock_config_storage, get_channel_information_for_machine(testing::A<const machine&>())).Times(1).WillRepeatedly(testing::Return(channels));

	const auto mock_data_storage = std::dynamic_pointer_cast<MockDataAccess>(data_storage);
	const auto types = ranges::views::iota(1, types_count + 1)
					   | ranges::views::transform([](const auto i) { return std::make_pair(i, std::to_string(i)); })
					   | ranges::to<std::vector>();

	EXPECT_CALL(*mock_data_storage, get_data_types_available_for_channel(testing::A<int>())).WillRepeatedly(testing::Return(types));

	std::vector<std::pair<time_point_t, float>> data = ranges::views::iota(0, data_point_count + 1)
														| ranges::views::transform([](const int i) {return std::make_pair(time_point_t(std::chrono::seconds(i)), static_cast<float>(i)); })
														| ranges::to<std::vector>();
	EXPECT_CALL(*mock_data_storage, get_data(testing::A<int>(), testing::A<int>(), testing::A<time_point_t>(), testing::A<time_point_t>()))
		.WillRepeatedly(testing::Return(data));

	auto aggregator = MLDataAggregator<300>(data_storage, config_storage);

	const auto start = std::chrono::high_resolution_clock::now();
	const auto aggregate_data = aggregator.get_data_before_time(1, time_point_t::max());
	const auto end = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms. " << std::endl;
	ASSERT_LE(end - start, 1s);
}