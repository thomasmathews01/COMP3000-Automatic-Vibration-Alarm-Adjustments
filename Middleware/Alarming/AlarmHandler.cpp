#include "AlarmHandler.h"
#include <utility>
#include <range/v3/all.hpp>
#include <boost/hof.hpp>
#include "taskflow.hpp"

using namespace ranges;

void AlarmHandler::process() {
    tf::Taskflow taskflow;
    tf::Executor executor; // Generate a thread pool with one thread for each core on the machine we execute on.

    const auto update_time = internal_clock->get_current_time();
    taskflow.for_each(statistics.begin(), statistics.end(), [update_time](auto& alarm) { alarm->update(update_time); });
    taskflow.for_each(alarms.begin(), alarms.end(), [update_time](auto& alarm) { alarm->update(update_time); });

    executor.run(taskflow).wait(); // Split all our updates across threads since they are independent.
}

void AlarmHandler::load_alarms() {
    const auto all_settings = alarm_storage->get_all_alarm_settings();
    const auto constructor = boost::hof::partial(&IAlarmCalcFactory::construct_calculator)(alarm_factory)(data_storage)(alarm_storage);

    alarms |= actions::push_back(all_settings | views::transform(constructor));
}

void AlarmHandler::load_statistics() {
    const auto all_type_ids = data_storage->get_all_data_types();

    const auto all_channels = configuration_storage->get_all_channels();
    auto all_channel_ids = all_channels | views::transform(&channel::id);

    auto all_calculators = views::cartesian_product(all_channel_ids, all_type_ids) // Generate all unique combinations of channel and type ID.
                           | views::transform([this](const auto pair) { return get_statistics_calculator_for_pair(pair); });

    statistics |= actions::push_back(all_calculators);
}

std::shared_ptr<IAlarmStatistics> AlarmHandler::get_statistics_calculator_for_pair(const std::tuple<int, int>& channel_type_pair) {
    const auto&[channel, type] = channel_type_pair;
    return statistics_factory->get_statistics_calculator(channel, type, statistics_storage, data_storage);
}

AlarmHandler::AlarmHandler(std::shared_ptr<IClock> internal_clock, std::shared_ptr<IAlarmStorage> alarm_storage,
                           std::shared_ptr<IAlarmCalcFactory> factory, std::shared_ptr<IStatistics> statistics_storage,
                           std::shared_ptr<IDataAccess> data_storage, std::shared_ptr<IStatsCalcFactory> statistics_factory,
                           std::shared_ptr<IConfigurationAccess> configuration_storage)
        : internal_clock(std::move(internal_clock)), alarm_storage(std::move(alarm_storage)), alarm_factory(std::move(factory)),
          statistics_storage(std::move(statistics_storage)), data_storage(std::move(data_storage)), statistics_factory(std::move(statistics_factory)),
          configuration_storage(std::move(configuration_storage)) {
    load_alarms();
    load_statistics();
}