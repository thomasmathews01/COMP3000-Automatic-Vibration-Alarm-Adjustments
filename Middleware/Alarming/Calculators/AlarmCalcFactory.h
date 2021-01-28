#pragma once

#include "IAlarmCalcFactory.h"

class AlarmCalcFactory : public IAlarmCalcFactory {
public:
	virtual std::shared_ptr<IAlarmCalc> construct_calculator(std::shared_ptr<IDatabase> database, const alarm_settings_t& alarm_settings) override;
};


