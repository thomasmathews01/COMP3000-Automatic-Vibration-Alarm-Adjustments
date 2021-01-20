#include "AlarmCalculatorFactory.h"

#include <utility>
#include "Calculators/FixedAlarmCalculator.h"

AlarmCalculatorFactory::AlarmCalculatorFactory(std::shared_ptr<IDatabase> database) : database(std::move(database)) {}

std::shared_ptr<IAlarmCalculator> AlarmCalculatorFactory::create(const alarm_settings_t& alarm) {
	return std::make_shared<FixedAlarmCalculator>(alarm, database);
}

