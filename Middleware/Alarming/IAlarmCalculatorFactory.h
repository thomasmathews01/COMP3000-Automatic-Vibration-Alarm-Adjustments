#pragma once

#include "Calculators/IAlarmCalculator.h"
#include "alarmTypes.h"

class IAlarmCalculatorFactory {
public:
	virtual std::shared_ptr<IAlarmCalculator> create(const alarm_settings_t& alarm) = 0;
};


