#pragma once

#include <memory>
#include "Calculators/IAlarmCalculator.h"
#include "IAlarmCalculatorFactory.h"
#include "alarmTypes.h"
#include "IDatabase.h"

class AlarmCalculatorFactory : public IAlarmCalculatorFactory {
public:
	explicit AlarmCalculatorFactory(std::shared_ptr<IDatabase> database);

	std::shared_ptr<IAlarmCalculator> create(const alarm_settings_t& alarm) final;

private:
	std::shared_ptr<IDatabase> database;
};


