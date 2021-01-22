#pragma once

#include <thread>
#include <vector>
#include <functional>
#include "Calculators/IAlarmCalculator.h"
#include "IAlarmCalculatorFactory.h"
#include "IDatabase.h"

class AlarmOrchestrator {
public:
	explicit AlarmOrchestrator(std::shared_ptr<IDatabase> database);
	~AlarmOrchestrator();

private:
	std::vector<std::shared_ptr<IAlarmCalculator>> alarm_calculators;
	std::vector<std::function<void()>> get_update_tasks();

	std::shared_ptr<IDatabase> database;
	std::shared_ptr<IAlarmCalculatorFactory> calculator_factory;

	std::atomic<bool> should_stop = false;
	std::thread worker_thread;

	void update_alarm_states();
	void load_initial_alarm_calculators();
};


