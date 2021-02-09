#include <gtest/gtest.h>
#include "Stubs/StubDataAccess.h"
#include "Stubs/StubStatistics.h"
#include "Stubs/StubConfigurationAccess.h"
#include "Stubs/StubClock.h"
#include "Statistics/AlarmStatistics.h"
#include <Hypodermic/Hypodermic.h>
#include <array>
#include <range/v3/all.hpp>
#include "Statistics/SOM.h"
#include "Statistics/SOMLearningFunctions.h"

using namespace ranges;

namespace {
	template<class T>
	nanoseconds time_lambda(T&& lambda)
	{
		const auto start = std::chrono::high_resolution_clock::now();
		lambda();

		const auto change = std::chrono::high_resolution_clock::now() - start;
		std::cout << "Function took: " << duration_cast<nanoseconds>(change).count() << "ns. " << std::endl;

		return change;
	}
}

std::shared_ptr<Hypodermic::Container> get_container() {
	Hypodermic::ContainerBuilder builder;

	builder.registerType<StubStatistics>().as<IStatistics>().singleInstance();
	builder.registerType<StubClock>().as<IClock>().singleInstance();
	builder.registerType<StubDataAccess>().as<IDataAccess>().singleInstance();
	builder.registerType<StubConfigurationAccess>().as<IConfigurationAccess>().singleInstance();

	return builder.build();
}

TEST (Statistics, canConstructStatisticsObject) {
	alarm_settings_t test_settings(1, 2, alarmSeverity::alarm);
	const auto container = get_container();

	ASSERT_NO_THROW(AlarmStatistics(test_settings.channel_id, test_settings.type_id, container->resolve<IStatistics>(), container->resolve<IDataAccess>()));
}

TEST (Statistics, CalculatesMeanOfSingleValue) {
	alarm_settings_t test_settings(1, 2, alarmSeverity::alarm);
	const auto container = get_container();

	auto test_object = AlarmStatistics(test_settings.channel_id, test_settings.type_id, container->resolve<IStatistics>(), container->resolve<IDataAccess>());

	const auto stub_data_access = std::dynamic_pointer_cast<StubDataAccess>(container->resolve<IDataAccess>());
	stub_data_access->data.emplace_back(time_point_t(100s), 10.f);
	stub_data_access->data.emplace_back(time_point_t(101s), 10.f);
	stub_data_access->data.emplace_back(time_point_t(102s), 10.f);
	test_object.update(container->resolve<IClock>()->get_current_time());

	const auto stub_statistics_storage = std::dynamic_pointer_cast<StubStatistics>(container->resolve<IStatistics>());
	ASSERT_EQ(10.f, stub_statistics_storage->point.mean);
}


TEST (Statistics, CalculatesMeanOf1ThousandValuesInAcceptableTime) {
	alarm_settings_t test_settings(1, 2, alarmSeverity::alarm);
	const auto container = get_container();

	auto test_object = AlarmStatistics(test_settings.channel_id, test_settings.type_id, container->resolve<IStatistics>(), container->resolve<IDataAccess>());

	const auto stub_data_access = std::dynamic_pointer_cast<StubDataAccess>(container->resolve<IDataAccess>());

	stub_data_access->data |= actions::push_back(views::iota(1, 1000) | views::transform([](const auto i) { return std::make_pair(time_point_t(seconds(i)), i); }));

	const auto execution_time = time_lambda([&](){test_object.update(container->resolve<IClock>()->get_current_time()); });
	ASSERT_LE((duration_cast<milliseconds>(execution_time)).count(), (10ms).count());
}

TEST (Statistics, CanCalculateRollingMean) {
	alarm_settings_t test_settings(1, 2, alarmSeverity::alarm);
	const auto container = get_container();

	auto test_object = AlarmStatistics(test_settings.channel_id, test_settings.type_id, container->resolve<IStatistics>(), container->resolve<IDataAccess>());

	const auto stub_data_access = std::dynamic_pointer_cast<StubDataAccess>(container->resolve<IDataAccess>());

	stub_data_access->data |= actions::push_back(views::iota(1, 6) | views::transform([](const auto i) { return std::make_pair(time_point_t(seconds(i)), i); }));

	test_object.update(container->resolve<IClock>()->get_current_time());

	stub_data_access->data.clear();
	stub_data_access->data |= actions::push_back(views::iota(6, 11) | views::transform([](const auto i) { return std::make_pair(time_point_t(seconds(i)), i); }));

	test_object.update(container->resolve<IClock>()->get_current_time());

	const auto stub_statistics_storage = std::dynamic_pointer_cast<StubStatistics>(container->resolve<IStatistics>());
	ASSERT_EQ(5.5, stub_statistics_storage->point.mean);
}

TEST (Statistics, CanCalculateStdDeviation) {
	alarm_settings_t test_settings(1, 2, alarmSeverity::alarm);
	const auto container = get_container();

	auto test_object = AlarmStatistics(test_settings.channel_id, test_settings.type_id, container->resolve<IStatistics>(), container->resolve<IDataAccess>());

	const auto stub_data_access = std::dynamic_pointer_cast<StubDataAccess>(container->resolve<IDataAccess>());

	stub_data_access->data |= actions::push_back(views::iota(1, 11) | views::transform([](const auto i) { return std::make_pair(time_point_t(seconds(i)), i); }));

	test_object.update(container->resolve<IClock>()->get_current_time());

	const auto stub_statistics_storage = std::dynamic_pointer_cast<StubStatistics>(container->resolve<IStatistics>());
	ASSERT_EQ(8.25, stub_statistics_storage->point.variance);
}

TEST (Statistics, CanCalculateRollingStdDeviation) {
	alarm_settings_t test_settings(1, 2, alarmSeverity::alarm);
	const auto container = get_container();

	auto test_object = AlarmStatistics(test_settings.channel_id, test_settings.type_id, container->resolve<IStatistics>(), container->resolve<IDataAccess>());

	const auto stub_data_access = std::dynamic_pointer_cast<StubDataAccess>(container->resolve<IDataAccess>());

	stub_data_access->data |= actions::push_back(views::iota(1, 6) | views::transform([](const auto i) { return std::make_pair(time_point_t(seconds(i)), i); }));
	ASSERT_EQ(5, stub_data_access->data.size());

	test_object.update(container->resolve<IClock>()->get_current_time());

	stub_data_access->data.clear();
	stub_data_access->data |= actions::push_back(views::iota(6, 11) | views::transform([](const auto i) { return std::make_pair(time_point_t(seconds(i)), i); }));
	ASSERT_EQ(5, stub_data_access->data.size());

	test_object.update(container->resolve<IClock>()->get_current_time());

	const auto stub_statistics_storage = std::dynamic_pointer_cast<StubStatistics>(container->resolve<IStatistics>());
	ASSERT_EQ(8.25, stub_statistics_storage->point.variance);
}

TEST (Statistics, somdoesntdieimmediately) {
	using som_type_t = SOM<100, 100, 1000>;
	som_type_t som;
	som.initialise();
	som.train(std::vector<som_type_t::som_point_t>{}, LearningFunctions::get_inverse_time_learning_function(som_type_t::iterations), NeighbourhoodFunctions::get_inverse_time_neighbourhood_function(som_type_t::iterations, som_type_t::map_size));
}
