#pragma once
#include <memory>
#include <IDataAccess.h>
#include <IAlarmStorage.h>

#include "IAlarmCalc.h"

class IAlarmCalcFactory {
public:
	virtual std::shared_ptr<IAlarmCalc> construct_calculator(std::shared_ptr<IDataAccess> data_access, std::shared_ptr<IAlarmStorage> alarm_access, const alarm_settings_t& alarm_settings) = 0;
};


