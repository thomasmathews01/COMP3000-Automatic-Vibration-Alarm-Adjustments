#include "AlarmHandler.h"
#include <utility>
#include "taskflow.hpp"

void AlarmHandler::process() {
    tf::Executor executor;
    tf::Taskflow taskflow;

    for (auto &calc : statistics)
        taskflow.emplace(
                [this, &calc]() { calc->update(internal_clock->get_current_time()); });

    for (auto &alarm_calc : alarms)
        taskflow.emplace(
                [this, &alarm_calc]() { alarm_calc->update_alarm_state_at(internal_clock->get_current_time()); });

    executor.run(taskflow).wait();
}

void AlarmHandler::load_alarms() {
    const auto all_settings = database->get_all_alarm_settings();
    std::transform(all_settings.cbegin(), all_settings.cend(), std::back_inserter(alarms),
                   [this](const auto &setting) { return alarm_factory->construct_calculator(database, setting); });
}

void AlarmHandler::load_statistics() {
    const auto all_settings = database->get_all_alarm_settings();
    std::transform(all_settings.cbegin(), all_settings.cend(), std::back_inserter(statistics),
                   [this](const auto &setting) { return statistics_factory->get_statistics_calculator(setting, statistics_storage, data_storage); });
}


AlarmHandler::AlarmHandler(std::shared_ptr<IClock> internal_clock, std::shared_ptr<IDatabase> database,
                           std::shared_ptr<IAlarmCalcFactory> factory, std::shared_ptr<IStatistics> statistics_storage,
                           std::shared_ptr<IDataAccess> data_storage, std::shared_ptr<IStatsCalcFactory> statistics_factory)
        : internal_clock(std::move(internal_clock)), database(std::move(database)), alarm_factory(std::move(factory)),
          statistics_storage(statistics_storage), data_storage(data_storage), statistics_factory(statistics_factory) {
    load_alarms();
    load_statistics();
}
