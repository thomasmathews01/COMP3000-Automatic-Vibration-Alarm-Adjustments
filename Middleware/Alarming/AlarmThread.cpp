#include "AlarmThread.h"
#include "RealClock.h"
#include "Calculators/AlarmCalcFactory.h"
#include "Statistics/StatsCalcFactory.h"

AlarmThread::AlarmThread(const std::shared_ptr<IDatabase>& database) {
	auto clock = std::make_shared<RealClock>();
	auto factory = std::make_shared<AlarmCalcFactory>();
	auto stats_factory = std::make_shared<StatsCalcFactory>();

	processor = std::make_shared<AlarmHandler>(clock, database, factory, database, database, stats_factory);
	worker = std::thread([this]() { process_alarms(); });
}

void AlarmThread::process_alarms() {
	while (!should_stop) {
		const auto start = std::chrono::steady_clock::now();
		processor->process();
		std::this_thread::sleep_until(start + process_loop);
	}
}


AlarmThread::~AlarmThread() {
	should_stop = true;
	if (worker.joinable())
		worker.join();
}