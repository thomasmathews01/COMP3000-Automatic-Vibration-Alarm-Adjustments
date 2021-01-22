#include "AlarmOrchestrator.h"
#include <range/v3/all.hpp>
#include <chrono>
#include <iostream>
#include <functional>
#include <utility>
#include <taskflow.hpp>

using namespace ranges;
using namespace std::chrono_literals;

namespace
{
	auto get_update_tasks_for_time(std::shared_ptr<IDatabase> database, std::vector<std::shared_ptr<IAlarmCalculator>>& alarm_calculators, const time_point_t time) {
		return alarm_calculators
			   | views::transform([database, time](auto& calculator)
								  {
									  return [&]()
									  {
										  const auto new_state = calculator->get_alarm_state_at(time);
										  const auto previous_state = database->get_alarm_state_of_alarm(calculator->get_alarm_settings());

										  if (previous_state.severity != new_state.severity)
											  database->add_alarm_activation(alarm_activation_t(calculator->get_alarm_settings(), time, new_state));
									  };
								  });
	}
}

AlarmOrchestrator::AlarmOrchestrator(std::shared_ptr<IDatabase> database) : database(std::move(database)) {
	should_stop = false;
	worker_thread = std::thread([this]() { update_alarm_states(); });
}

AlarmOrchestrator::~AlarmOrchestrator() {
	should_stop = true;

	if (worker_thread.joinable())
		worker_thread.join();
}

void AlarmOrchestrator::update_alarm_states() {
	load_initial_alarm_calculators();
	tf::Executor executor;

	while (!should_stop) {
		tf::Taskflow taskflow;

		for (const auto& update_task : get_update_tasks_for_time(database, alarm_calculators, time_point_t(0s)))
			taskflow.emplace(update_task);

		executor.run(taskflow).wait();
	}


	/*
	 * This is all nonsense because they can all be updated whilst we are running, so the task dependency model can change, meaning it needs rebuilding every loop
	 * This makes taskflow more useful, because we can encode the statistical dependencies, but it means we can't generate either the calculators or the statistical
	 * calculation modules up front, since we have no idea where they are going to be needed.
	 */
}

void AlarmOrchestrator::load_initial_alarm_calculators() {
	const auto alarm_settings = database->get_all_alarm_settings();

	alarm_calculators |= actions::push_back(alarm_settings
											| views::transform([this](const auto& alarm) { return calculator_factory->create(alarm); }));
}
