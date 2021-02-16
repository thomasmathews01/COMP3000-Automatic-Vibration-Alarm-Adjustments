#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <range/v3/all.hpp>
#include <DataStorage.h>
#include <ConfigStorage.h>
#include <STLExtensions.h>
#include "AllMocks.h"
#include <NetworkRequestHandlers/CurrentAlarmStateRequest.h>

using namespace ranges;
using namespace std::chrono_literals;

class CurrentAlarmStateTest : public ::testing::Test {
protected:
	void SetUp() override {
		alarm_storage = std::make_shared<MockAlarmStorage>();
		config_storage = std::make_shared<MockConfigurationAccess>();
	}

	void TearDown() override {

	}

	std::shared_ptr<IAlarmStorage> alarm_storage;
	std::shared_ptr<IConfigurationAccess> config_storage;

	auto get_response(std::optional<int> machine, std::optional<int> channel, std::optional<int> type) {
		std::string url_string = "/request?";
		if (machine) url_string += "machine_id=" + std::to_string(*machine);
		if (channel) url_string += "&channel_id=" + std::to_string(*channel);
		if (type) url_string += "&type_id=" + std::to_string(*type);

		auto request = crow::request();
		request.url_params = crow::query_string(url_string);

		return CurrentAlarmStateRequest::get_current_alarm_state(request, config_storage, alarm_storage);
	}

	template<typename Cn>
	int count_occurances_of_substring(const Cn& container, const std::string_view sub_string) {
		return STLExtensions::count_occurances_of_subrange(container.cbegin(), container.cend(), sub_string.cbegin(), sub_string.cend());
	}
};

TEST_F(CurrentAlarmStateTest, CanCreateMocks) {
	EXPECT_TRUE(true);
}

TEST_F(CurrentAlarmStateTest, CanExecuteWithoutThrowing) {
	auto mock_config_storage = std::dynamic_pointer_cast<MockConfigurationAccess>(config_storage);
	EXPECT_CALL(*mock_config_storage, get_machine_id_from_channel_id(2)).WillRepeatedly(testing::Return(5));
	auto mock_alarm_storage = std::dynamic_pointer_cast<MockAlarmStorage>(alarm_storage);
	EXPECT_CALL(*mock_alarm_storage, get_activations_for_machine(1)).Times(1).WillOnce(testing::Return(std::vector<alarm_activation_t>
																										   {
																											   alarm_activation_t(2, 1, alarmSeverity::alarm, time_point_t::max() - 5s, true),
																											   alarm_activation_t(2, 1, alarmSeverity::alarm, time_point_t::max() - 4s, false),
																											   alarm_activation_t(2, 2, alarmSeverity::alarm, time_point_t::max() - 3s, true),
																											   alarm_activation_t(2, 2, alarmSeverity::alarm, time_point_t::max() - 2s, false),
																											   alarm_activation_t(2, 2, alarmSeverity::alarm, time_point_t::max() - 1s, true)
																										   }));

	const auto activations = get_response(1, 1, std::nullopt);
	EXPECT_EQ(200, activations.code);
}

TEST_F(CurrentAlarmStateTest, CanGetAlarmStateOfMachine) {
	auto mock_config_storage = std::dynamic_pointer_cast<MockConfigurationAccess>(config_storage);
	EXPECT_CALL(*mock_config_storage, get_machine_id_from_channel_id(2)).Times(0);
	auto mock_alarm_storage = std::dynamic_pointer_cast<MockAlarmStorage>(alarm_storage);
	EXPECT_CALL(*mock_alarm_storage, get_activations_for_machine(1)).Times(1).WillOnce(testing::Return(std::vector<alarm_activation_t>
																										   {
																											   alarm_activation_t(5, 1, alarmSeverity::alert, time_point_t::max() - 5s, true),
																											   alarm_activation_t(5, 1, alarmSeverity::alert, time_point_t::max() - 4s, false),
																											   alarm_activation_t(5, 2, alarmSeverity::alert, time_point_t::max() - 3s, true),
																											   alarm_activation_t(5, 2, alarmSeverity::alert, time_point_t::max() - 2s, false),
																											   alarm_activation_t(5, 2, alarmSeverity::alert, time_point_t::max() - 1s, true)
																										   }));

	const auto activations = get_response(1, 5, std::nullopt);
	EXPECT_EQ(200, activations.code);
	EXPECT_EQ(1, count_occurances_of_substring(activations.body, "alert"));
}

TEST_F(CurrentAlarmStateTest, MachineAlarmStatePicksHighestAlarmLevel) {
	auto mock_config_storage = std::dynamic_pointer_cast<MockConfigurationAccess>(config_storage);
	EXPECT_CALL(*mock_config_storage, get_machine_id_from_channel_id(2)).Times(0);
	auto mock_alarm_storage = std::dynamic_pointer_cast<MockAlarmStorage>(alarm_storage);
	EXPECT_CALL(*mock_alarm_storage, get_activations_for_machine(1)).Times(1).WillOnce(testing::Return(std::vector<alarm_activation_t>
																										   {
																											   alarm_activation_t(5, 1, alarmSeverity::alert, time_point_t::max() - 5s, true),
																											   alarm_activation_t(5, 1, alarmSeverity::alert, time_point_t::max() - 4s, false),
																											   alarm_activation_t(5, 2, alarmSeverity::alarm, time_point_t::max() - 3s, true),
																											   alarm_activation_t(5, 2, alarmSeverity::alert, time_point_t::max() - 2s, false),
																											   alarm_activation_t(5, 2, alarmSeverity::alert, time_point_t::max() - 1s, true)
																										   }));

	const auto activations = get_response(1, 5, std::nullopt);
	EXPECT_EQ(200, activations.code);
	EXPECT_EQ(1, count_occurances_of_substring(activations.body, "alarm"));
}

TEST_F(CurrentAlarmStateTest, CanGetAlarmLevelOfSpecificChannel) {
	auto mock_config_storage = std::dynamic_pointer_cast<MockConfigurationAccess>(config_storage);
	EXPECT_CALL(*mock_config_storage, get_machine_id_from_channel_id(2)).Times(0);
	auto mock_alarm_storage = std::dynamic_pointer_cast<MockAlarmStorage>(alarm_storage);
	EXPECT_CALL(*mock_alarm_storage, get_activations_for_machine(1)).Times(1).WillOnce(testing::Return(std::vector<alarm_activation_t>
																										   {
																											   alarm_activation_t(5, 1, alarmSeverity::alert, time_point_t::max() - 5s, true),
																											   alarm_activation_t(5, 1, alarmSeverity::alert, time_point_t::max() - 4s, false),
																											   alarm_activation_t(3, 1, alarmSeverity::alarm, time_point_t::max() - 3s, true),
																											   alarm_activation_t(3, 1, alarmSeverity::alarm, time_point_t::max() - 2s, false),
																											   alarm_activation_t(3, 1, alarmSeverity::alarm, time_point_t::max() - 1s, true)
																										   }));

	const auto activations = get_response(1, 5, std::nullopt);
	EXPECT_EQ(200, activations.code);
	EXPECT_EQ(1, count_occurances_of_substring(activations.body, "alert"));
}
