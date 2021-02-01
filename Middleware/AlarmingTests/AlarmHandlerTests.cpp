#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <array>
#include "Statistics/AlarmStatistics.h"
#include "AlarmHandler.h"
#include <Hypodermic/Hypodermic.h>
#include <boost/hof/construct.hpp>
#include "Mocks/AllMocks.h"
#include "taskflow.hpp"
#include <range/v3/all.hpp>
#include <range/v3/view/iota.hpp>

using namespace ::testing;
using namespace std::chrono;
using namespace std::chrono_literals;

std::shared_ptr<Hypodermic::Container> get_test_container() {
    Hypodermic::ContainerBuilder builder;

    builder.registerType<MockStatistics>().as<IStatistics>().singleInstance();
    builder.registerType<MockClock>().as<IClock>().singleInstance();
    builder.registerType<MockDataAccess>().as<IDataAccess>().singleInstance();
    builder.registerType<MockAlarmStorage>().as<IAlarmStorage>().singleInstance();
    builder.registerType<MockConfigurationAccess>().as<IConfigurationAccess>().singleInstance();
    builder.registerType<MockAlarmCalcFactory>().as<IAlarmCalcFactory>().singleInstance();
    builder.registerType<MockStatsCalcFactory>().as<IStatsCalcFactory>().singleInstance();

    return builder.build();
}

TEST (AlarmHandlerTests, canConstructAlarmHandlerObject) {
    const auto container = get_test_container();

    ASSERT_NO_THROW(AlarmHandler(container->resolve<IClock>(), container->resolve<IAlarmStorage>(), container->resolve<IAlarmCalcFactory>(), container->resolve<IStatistics>(),
                                 container->resolve<IDataAccess>(), container->resolve<IStatsCalcFactory>(), container->resolve<IConfigurationAccess>()));
}

TEST (AlarmHandlerTests, LoadsStatisticsCalculatorsForAllChannelTypeCombinations) {
    const auto container = get_test_container();

    const auto mock_config_access = std::dynamic_pointer_cast<MockConfigurationAccess>(container->resolve<IConfigurationAccess>());
    const std::vector<channel> channels = {{1, "name",  "units"},
                                           {2, "name2", "units2"}};
    EXPECT_CALL(*mock_config_access, get_all_channels()).Times(1).WillRepeatedly(testing::Return(channels));

    const auto mock_data_access = std::dynamic_pointer_cast<MockDataAccess>(container->resolve<IDataAccess>());
    const std::vector<int> data_types = {1, 2, 3, 4, 5};
    EXPECT_CALL(*mock_data_access, get_all_data_types()).Times(1).WillRepeatedly(testing::Return(data_types));

    const auto mock_stats_factory = std::dynamic_pointer_cast<MockStatsCalcFactory>(container->resolve<IStatsCalcFactory>());
    EXPECT_CALL(*mock_stats_factory, get_statistics_calculator(_, _, _, _)).Times(data_types.size() * channels.size());

    ASSERT_NO_THROW(AlarmHandler(container->resolve<IClock>(), container->resolve<IAlarmStorage>(), container->resolve<IAlarmCalcFactory>(), container->resolve<IStatistics>(),
                                 container->resolve<IDataAccess>(), container->resolve<IStatsCalcFactory>(), container->resolve<IConfigurationAccess>()));
}

TEST (AlarmHandlerTests, LoadsCalculatorsForAllAlarms) {
    const auto container = get_test_container();

    const auto mock_config_access = std::dynamic_pointer_cast<MockConfigurationAccess>(container->resolve<IConfigurationAccess>());
    const std::vector<channel> channels = {{1, "name",  "units"},
                                           {2, "name2", "units2"}};

    EXPECT_CALL(*mock_config_access, get_all_channels()).Times(1).WillRepeatedly(testing::Return(channels));

    const auto mock_alarm_access = std::dynamic_pointer_cast<MockAlarmStorage>(container->resolve<IAlarmStorage>());
    const std::vector<alarm_settings_t> alarm_settings = {{1, 1, alarmSeverity::alarm},
                                                          {1, 1, alarmSeverity::alert}};
    EXPECT_CALL(*mock_alarm_access, get_all_alarm_settings()).Times(1).WillRepeatedly(testing::Return(alarm_settings));

    const auto mock_alarm_calc_factory = std::dynamic_pointer_cast<MockAlarmCalcFactory>(container->resolve<IAlarmCalcFactory>());
    EXPECT_CALL(*mock_alarm_calc_factory, construct_calculator(_, _, _)).Times(alarm_settings.size());

    ASSERT_NO_THROW(AlarmHandler(container->resolve<IClock>(), container->resolve<IAlarmStorage>(), container->resolve<IAlarmCalcFactory>(), container->resolve<IStatistics>(),
                                 container->resolve<IDataAccess>(), container->resolve<IStatsCalcFactory>(), container->resolve<IConfigurationAccess>()));
}

TEST (AlarmHandlerTests, ProcessesAllAlarmsWithCurrentTime) {
    const auto container = get_test_container();
    const auto time = time_point_t(100s);
    EXPECT_CALL(*(std::dynamic_pointer_cast<MockClock>(container->resolve<IClock>())), get_current_time).WillRepeatedly(Return(time));

    const auto mock_alarm_access = std::dynamic_pointer_cast<MockAlarmStorage>(container->resolve<IAlarmStorage>());
    std::vector<alarm_settings_t> alarm_settings;
    std::generate_n(std::back_inserter(alarm_settings), 5, [i = 1]() mutable { return alarm_settings_t{i, i++, alarmSeverity::alarm}; });
    EXPECT_CALL(*mock_alarm_access, get_all_alarm_settings()).Times(1).WillRepeatedly(testing::Return(alarm_settings));

    auto calculators = ranges::views::generate([time]() {
        auto mock = std::make_shared<MockAlarmCalc>();
        EXPECT_CALL(*mock, update(time)).Times(AtLeast(1));
        return std::dynamic_pointer_cast<IAlarmCalc>(mock);
    }) | ranges::views::take_exactly(5) | ranges::to<std::vector>();

    const auto mock_alarm_calc_factory = std::dynamic_pointer_cast<MockAlarmCalcFactory>(container->resolve<IAlarmCalcFactory>());
    EXPECT_CALL(*mock_alarm_calc_factory, construct_calculator(_, _, _))
            .WillOnce(Return(calculators.at(0)))
            .WillOnce(Return(calculators.at(1)))
            .WillOnce(Return(calculators.at(2)))
            .WillOnce(Return(calculators.at(3)))
            .WillOnce(Return(calculators.at(4)));

    auto handler = AlarmHandler(container->resolve<IClock>(), container->resolve<IAlarmStorage>(), container->resolve<IAlarmCalcFactory>(), container->resolve<IStatistics>(),
                                container->resolve<IDataAccess>(), container->resolve<IStatsCalcFactory>(), container->resolve<IConfigurationAccess>());

    handler.process();
}

TEST (AlarmHandlerTests, ProcessesAllStatisticsWithCurrentTime) {
    const auto container = get_test_container();
    const auto time = time_point_t(100s);
    EXPECT_CALL(*(std::dynamic_pointer_cast<MockClock>(container->resolve<IClock>())), get_current_time).WillRepeatedly(Return(time));

    const auto mock_config_access = std::dynamic_pointer_cast<MockConfigurationAccess>(container->resolve<IConfigurationAccess>());
    const std::vector<channel> channels = {{1, "name",  "units"},
                                           {2, "name2", "units2"}};

    EXPECT_CALL(*mock_config_access, get_all_channels()).Times(1).WillRepeatedly(testing::Return(channels));

    const auto mock_data_access = std::dynamic_pointer_cast<MockDataAccess>(container->resolve<IDataAccess>());
    const std::vector<int> data_types = {1, 2};
    EXPECT_CALL(*mock_data_access, get_all_data_types()).Times(1).WillRepeatedly(testing::Return(data_types));

    auto calculators = ranges::views::generate([time]() {
        auto mock = std::make_shared<MockAlarmStatistics>();
        EXPECT_CALL(*mock, update(time)).Times(AtLeast(1));
        return mock;
    }) | ranges::views::take_exactly(4) | ranges::to<std::vector>();

    const auto mock_alarm_calc_factory = std::dynamic_pointer_cast<MockStatsCalcFactory>(container->resolve<IStatsCalcFactory>());
    EXPECT_CALL(*mock_alarm_calc_factory, get_statistics_calculator(_, _, _, _))
            .WillOnce(Return(calculators.at(0)))
            .WillOnce(Return(calculators.at(1)))
            .WillOnce(Return(calculators.at(2)))
            .WillOnce(Return(calculators.at(3)));

    auto handler = AlarmHandler(container->resolve<IClock>(), container->resolve<IAlarmStorage>(), container->resolve<IAlarmCalcFactory>(), container->resolve<IStatistics>(),
                                container->resolve<IDataAccess>(), container->resolve<IStatsCalcFactory>(), container->resolve<IConfigurationAccess>());

    handler.process();
}