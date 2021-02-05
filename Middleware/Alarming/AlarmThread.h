#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include "AlarmHandler.h"

using namespace std::chrono_literals;
using namespace std::chrono;

class AlarmThread {
public:
	AlarmThread(const std::shared_ptr<IAlarmStorage>& alarm_storage, const std::shared_ptr<IConfigurationAccess>& config_storage, const std::shared_ptr<IDataAccess>& data_storage, const std::shared_ptr<IStatistics>&
	        statics_storage);
	~AlarmThread();

	void process_alarms();
private:
	std::thread worker;
	std::atomic<bool> should_stop;
	std::shared_ptr<AlarmHandler> processor;
	constexpr static milliseconds process_loop = 10s;
};


