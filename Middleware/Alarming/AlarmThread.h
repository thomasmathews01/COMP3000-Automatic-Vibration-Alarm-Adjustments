#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include "InternalClock.h"
#include "AlarmHandler.h"

using namespace std::chrono_literals;
using namespace std::chrono;

class AlarmThread {
public:
	AlarmThread(const std::shared_ptr<IDatabase>& database, const std::shared_ptr<IConfigurationAccess>& config_storage);
	~AlarmThread();

	void process_alarms();
private:

	std::thread worker;
	std::atomic<bool> should_stop;
	std::shared_ptr<AlarmHandler> processor;
	milliseconds process_loop = 10s;
};


