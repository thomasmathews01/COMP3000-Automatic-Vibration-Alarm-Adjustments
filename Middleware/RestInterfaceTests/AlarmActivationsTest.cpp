#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <range/v3/all.hpp>
#include <DatabaseFactory.h>
#include <AlarmStorage.h>
#include <DataStorage.h>
#include <ConfigStorage.h>
#include <STLExtensions.h>
#include "AllMocks.h"
#include <NetworkRequestHandlers/AlarmActivationsRequest.h>

using namespace ranges;
using namespace std::chrono_literals;

class AlarmActivationsTest : public ::testing::Test {
protected:
	void SetUp() override {
		alarm_storage = std::make_shared<MockAlarmStorage>();
		config_storage = std::make_shared<MockConfigurationAccess>();
	}

	void TearDown() override {

	}

	std::shared_ptr<IAlarmStorage> alarm_storage;
	std::shared_ptr<IConfigurationAccess> config_storage;

	auto get_response(std::optional<int> machine, std::optional<int> channel, std::optional<int> type, alarmSeverity severity) {
		std::string url_string = "/request?";
		if (machine) url_string += "machine_id=" + std::to_string(*machine);
		if (channel) url_string += "&channel_id=" + std::to_string(*channel);
		if (type) url_string += "&type_id=" + std::to_string(*type);

		auto request = crow::request();
		request.url_params = crow::query_string(url_string);

		return AlarmActivationsRequest::get_activations(request, severity, config_storage, alarm_storage);
	}

	template<typename It>
	int count_occurances_of_substring(It first, It second, const std::string_view sub_string) {
		return STLExtensions::count_occurances_of_subrange(first, second, sub_string.cbegin(), sub_string.cend());
	}
};

TEST_F(AlarmActivationsTest, CanCreateMocks) {
	EXPECT_TRUE(true);
}

TEST_F(AlarmActivationsTest, CanExecuteWithoutThrowing) {
	const auto activations = get_response(1, 1, 1, alarmSeverity::alarm);
	EXPECT_EQ(200, activations.code);
}

TEST_F(AlarmActivationsTest, RequestActivationsForCorrectMachineWhenOnlyGivenChannel) {
	auto mock_config_storage = std::dynamic_pointer_cast<MockConfigurationAccess>(config_storage);
	EXPECT_CALL(*mock_config_storage, get_machine_id_from_channel_id(1)).WillRepeatedly(testing::Return(5));
	auto mock_alarm_storage = std::dynamic_pointer_cast<MockAlarmStorage>(alarm_storage);
	EXPECT_CALL(*mock_alarm_storage, get_activations_for_machine(5)).Times(1);

	const auto activations = get_response(std::nullopt, 1, 1, alarmSeverity::alarm);
	EXPECT_EQ(200, activations.code);
}

TEST_F(AlarmActivationsTest, CanFilterChannels) {
	auto mock_config_storage = std::dynamic_pointer_cast<MockConfigurationAccess>(config_storage);
	EXPECT_CALL(*mock_config_storage, get_machine_id_from_channel_id(2)).WillRepeatedly(testing::Return(5));
	auto mock_alarm_storage = std::dynamic_pointer_cast<MockAlarmStorage>(alarm_storage);
	EXPECT_CALL(*mock_alarm_storage, get_activations_for_machine(5)).Times(1).WillOnce(testing::Return(std::vector<alarm_activation_t>
		{
			alarm_activation_t(2, 1, alarmSeverity::alarm, time_point_t::max() - 5s, true),
			alarm_activation_t(2, 1, alarmSeverity::alarm, time_point_t::max() - 4s, false),
			alarm_activation_t(2, 1, alarmSeverity::alarm, time_point_t::max() - 3s, true),
			alarm_activation_t(3, 1, alarmSeverity::alarm, time_point_t::max() - 2s, false),
			alarm_activation_t(4, 1, alarmSeverity::alarm, time_point_t::max() - 1s, true)
		}));

	const auto activations = get_response(std::nullopt, 2, 1, alarmSeverity::alarm);
	EXPECT_EQ(200, activations.code);
	EXPECT_EQ(3, count_occurances_of_substring(activations.body.cbegin(), activations.body.cend(), "time"));
}

TEST_F(AlarmActivationsTest, CanFilterOnSensitivity) {
	auto mock_config_storage = std::dynamic_pointer_cast<MockConfigurationAccess>(config_storage);
	EXPECT_CALL(*mock_config_storage, get_machine_id_from_channel_id(2)).WillRepeatedly(testing::Return(5));
	auto mock_alarm_storage = std::dynamic_pointer_cast<MockAlarmStorage>(alarm_storage);
	EXPECT_CALL(*mock_alarm_storage, get_activations_for_machine(5)).Times(1).WillOnce(testing::Return(std::vector<alarm_activation_t>
		{
			alarm_activation_t(2, 1, alarmSeverity::alarm, time_point_t::max() - 5s, true),
			alarm_activation_t(2, 1, alarmSeverity::alert, time_point_t::max() - 4s, false),
			alarm_activation_t(2, 1, alarmSeverity::alert, time_point_t::max() - 3s, true),
			alarm_activation_t(2, 1, alarmSeverity::alert, time_point_t::max() - 2s, false),
			alarm_activation_t(2, 1, alarmSeverity::alarm, time_point_t::max() - 1s, true)
		}));

	const auto activations = get_response(std::nullopt, 2, 1, alarmSeverity::alarm);
	EXPECT_EQ(200, activations.code);
	EXPECT_EQ(2, count_occurances_of_substring(activations.body.cbegin(), activations.body.cend(), "time"));
}

TEST_F(AlarmActivationsTest, CanFilterOnTypes) {
	auto mock_config_storage = std::dynamic_pointer_cast<MockConfigurationAccess>(config_storage);
	EXPECT_CALL(*mock_config_storage, get_machine_id_from_channel_id(2)).WillRepeatedly(testing::Return(5));
	auto mock_alarm_storage = std::dynamic_pointer_cast<MockAlarmStorage>(alarm_storage);
	EXPECT_CALL(*mock_alarm_storage, get_activations_for_machine(5)).Times(1).WillOnce(testing::Return(std::vector<alarm_activation_t>
		{
			alarm_activation_t(2, 1, alarmSeverity::alarm, time_point_t::max() - 5s, true),
			alarm_activation_t(2, 1, alarmSeverity::alarm, time_point_t::max() - 4s, false),
			alarm_activation_t(2, 2, alarmSeverity::alarm, time_point_t::max() - 3s, true),
			alarm_activation_t(2, 2, alarmSeverity::alarm, time_point_t::max() - 2s, false),
			alarm_activation_t(2, 2, alarmSeverity::alarm, time_point_t::max() - 1s, true)
		}));

	const auto activations = get_response(std::nullopt, 2, 1, alarmSeverity::alarm);
	EXPECT_EQ(200, activations.code);
	EXPECT_EQ(2, count_occurances_of_substring(activations.body.cbegin(), activations.body.cend(), "time"));
}

TEST_F(AlarmActivationsTest, AsksForCorrectMachineIfMachineSpecified) {
	auto mock_alarm_storage = std::dynamic_pointer_cast<MockAlarmStorage>(alarm_storage);
	EXPECT_CALL(*mock_alarm_storage, get_activations_for_machine(5)).Times(1).WillOnce(testing::Return(std::vector<alarm_activation_t>
		{
			alarm_activation_t(1, 1, alarmSeverity::alarm, time_point_t::max() - 5s, true),
			alarm_activation_t(1, 1, alarmSeverity::alarm, time_point_t::max() - 4s, false),
			alarm_activation_t(1, 2, alarmSeverity::alarm, time_point_t::max() - 3s, true),
			alarm_activation_t(1, 2, alarmSeverity::alarm, time_point_t::max() - 2s, false),
			alarm_activation_t(1, 2, alarmSeverity::alarm, time_point_t::max() - 1s, true)
		}));

	const auto activations = get_response(5, 1, 1, alarmSeverity::alarm);
	EXPECT_EQ(200, activations.code);
}