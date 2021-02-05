#include "AlarmCalc.h"

#include <utility>

namespace
{
	constexpr float get_alarm_limit(const alarm_settings_t& settings) noexcept {
		switch (settings.threshold) {
			case alarmThreshold::automaticBasic: // TODO: Pull from statistics module
			case alarmThreshold::automaticStateBased: // TODO: Pull from statistics module
			case alarmThreshold::ISOLevel:
				return 100.f;
			case alarmThreshold::Custom:
				return settings.customLevel.value_or(100.f);
		}

		return 100.f;
	}
}


void AlarmCalc::update(const time_point_t& time) {
	alarm_settings = alarm_access->get_updated_alarm_settings(alarm_settings);
	const auto previous_state = alarm_access->get_alarm_state_of_alarm(alarm_settings);
	const auto [next_state, state_change_time] = get_next_state(time);

	if (previous_state != next_state)
		add_state_change(next_state, state_change_time);
}

std::pair<alarm_state_t, time_point_t> AlarmCalc::get_next_state(const time_point_t& time) const noexcept {
	const auto limit = get_alarm_limit(alarm_settings);

	const auto [last_value_time, last_value] = data_access->get_last_data_point_before(alarm_settings.channel_id, alarm_settings.type_id, time);

	const auto was_over_limit = last_value > limit;

	return {alarm_state_t(was_over_limit ? std::make_optional(alarm_settings.severity) : std::nullopt), last_value_time};
}

void AlarmCalc::add_state_change(const alarm_state_t& new_state, const time_point_t& time_of_state_change) const noexcept {
	const alarm_activation_t new_alarm_activation(alarm_settings, time_of_state_change, new_state);
	alarm_access->add_alarm_activation(new_alarm_activation);
}

AlarmCalc::AlarmCalc(std::shared_ptr<IDataAccess> data_access, std::shared_ptr<IAlarmStorage> alarm_access, const alarm_settings_t& alarm_settings)
	: data_access(std::move(data_access)), alarm_access(std::move(alarm_access)), alarm_settings(alarm_settings) {}
