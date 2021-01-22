#include "AlarmOrchestrator.h"
#include <range/v3/all.hpp>
#include <chrono>
#include <iostream>
#include <utility>
#include <taskflow.hpp>

using namespace ranges;
using namespace std::chrono_literals;

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
	tf::Taskflow taskflow;

	auto[A, B, C, D] = taskflow.emplace(  // create 4 tasks
		[]() { std::cout << "TaskA\n"; },
		[]() { std::cout << "TaskB\n"; },
		[]() { std::cout << "TaskC\n"; },
		[]() { std::cout << "TaskD\n"; }
	);

	A.precede(B, C);  // A runs before B and C
	D.succeed(B, C);  // D runs after  B and C

	executor.run(taskflow).wait();


	while (!should_stop) {
		for (const auto& calculator : alarm_calculators) {
			const auto new_state = calculator->get_alarm_state_at(time_point_t(0s));
			//database->update_alarm_state(calculator->get_alarm_settings(), new_state);

			if (new_state.severity.has_value())
				std::cout << "New state active";
		}
	}
}

void AlarmOrchestrator::load_initial_alarm_calculators() {
	for (const auto& site : database->get_site_data()) {
		for (const auto& machine : database->get_machines_for_site(site)) {
			const auto alarms = database->get_alarm_settings_for_machine(machine.id);

			std::transform(alarms.cbegin(), alarms.cend(), std::back_inserter(alarm_calculators), [this](const auto& alarm) { return calculator_factory->create(alarm); });
		}
	}
}
