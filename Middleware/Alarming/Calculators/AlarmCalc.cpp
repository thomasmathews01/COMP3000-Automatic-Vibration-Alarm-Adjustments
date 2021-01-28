#include "AlarmCalc.h"

#include <utility>

void AlarmCalc::update_alarm_state_at(const time_point_t& time) {
	alarm_settings = database->get_updated_alarm_settings(alarm_settings);
	const auto previous_state = database->get_alarm_state_of_alarm(alarm_settings);
	const auto[next_state, state_change_time] = get_next_state(time);

	if (previous_state != next_state) {
		add_state_change(next_state, state_change_time);
	}
}

std::pair<alarm_state_t, time_point_t> AlarmCalc::get_next_state(const time_point_t& time) {
	switch (alarm_settings.threshold) {
		case alarmThreshold::automaticBasic:
		case alarmThreshold::automaticStateBased:
            throw std::invalid_argument("Not implemented yet - do not support proper automatic alarming levels");
		case alarmThreshold::ISOLevel:
		case alarmThreshold::Custom:
			return get_state_of_alarm_fixed(time);
	}

    throw std::invalid_argument("Not implemented yet - do not support proper automatic alarming levels");
}

std::pair<alarm_state_t, time_point_t> AlarmCalc::get_state_of_alarm_fixed(const time_point_t& time) {
	const auto limit = alarm_settings.threshold == alarmThreshold::Custom ? alarm_settings.customLevel.value() : 100; // TODO: Replace with the actual ISO level from the vibration handbook.
	const auto[last_value_time, last_value] = database->get_last_data_point_before(alarm_settings.channel_id, alarm_settings.type_id, time);

	const auto was_over_limit = last_value > limit;

	return {alarm_state_t(was_over_limit ? std::make_optional(alarm_settings.severity) : std::nullopt), last_value_time};
}

void AlarmCalc::add_state_change(const alarm_state_t& new_state, const time_point_t& time_of_state_change) {
	const alarm_activation_t new_alarm_activation(alarm_settings, time_of_state_change, new_state);
	database->add_alarm_activation(new_alarm_activation);
}

AlarmCalc::AlarmCalc(std::shared_ptr<IDatabase> database, const alarm_settings_t& alarm_settings) : database(std::move(database)), alarm_settings(alarm_settings) {}
