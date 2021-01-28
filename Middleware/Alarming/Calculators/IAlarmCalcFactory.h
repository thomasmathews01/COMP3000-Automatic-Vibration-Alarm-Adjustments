#pragma once
#include <memory>
#include "IAlarmCalc.h"
#include "IDatabase.h"

class IAlarmCalcFactory {
public:
	virtual std::shared_ptr<IAlarmCalc> construct_calculator(std::shared_ptr<IDatabase> database, const alarm_settings_t& alarm_settings) = 0;
};


