#include "AlarmCalcFactory.h"
#include "AlarmCalc.h"

std::shared_ptr<IAlarmCalc> AlarmCalcFactory::construct_calculator(std::shared_ptr<IDatabase> database, const alarm_settings_t& alarm_settings) {
	return std::make_shared<AlarmCalc>(database, alarm_settings);
}
