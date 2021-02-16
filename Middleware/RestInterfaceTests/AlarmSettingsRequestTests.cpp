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

class AlarmSettingsRequestsTests : public ::testing::Test {
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

TEST_F(AlarmSettingsRequestsTests, CanCreateMocks) {
	EXPECT_TRUE(true);
}
