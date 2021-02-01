#include "AlarmCalcFactory.h"
#include "AlarmCalc.h"

std::shared_ptr<IAlarmCalc> AlarmCalcFactory::construct_calculator(std::shared_ptr<IDataAccess> data_access, std::shared_ptr<IAlarmStorage> alarm_access, const alarm_settings_t& alarm_settings) {
	return std::make_shared<AlarmCalc>(data_access, alarm_access, alarm_settings);
}
