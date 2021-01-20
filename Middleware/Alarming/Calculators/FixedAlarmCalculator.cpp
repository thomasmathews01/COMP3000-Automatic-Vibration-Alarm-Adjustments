#include "FixedAlarmCalculator.h"

FixedAlarmCalculator::FixedAlarmCalculator(const alarm_settings_t& alarm_settings, std::shared_ptr<IDatabase> database) : alarm_settings(alarm_settings), database(std::move(database)) {}

alarm_state_t FixedAlarmCalculator::get_alarm_state_at(const time_point_t& time) const noexcept {
	//const auto data_point = database->get_data(alarm_settings.channel_id, alarm_settings.type_id, ) // TODO: Add extra database call to facilitate this.

	const auto is_over_limit = 5.f > alarm_settings.customLevel.value();

	return alarm_state_t(is_over_limit ? std::make_optional(alarm_settings.severity) : std::nullopt);
}

alarm_settings_t FixedAlarmCalculator::get_alarm_settings() const noexcept {
	return alarm_settings;
}
