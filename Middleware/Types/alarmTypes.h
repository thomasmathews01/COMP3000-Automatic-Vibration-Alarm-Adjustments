#pragma once

#include "types.h"
#include <optional>
#include <stdexcept>
#include <string>

using namespace std::string_view_literals;

enum class alarmSeverity { // Ordered by severity. This matters!
	none,
	alert,
	alarm
};

inline std::string toString(const alarmSeverity severity) {
	switch (severity) {
		case alarmSeverity::alarm:
			return "alarm";
		case alarmSeverity::alert:
			return "alert";
		case alarmSeverity::none:
			return "none";
		default:
			throw std::invalid_argument("Unknown alarm severity");
	}
}

inline alarmSeverity parse_severity(const std::string_view severity_string) {
	if (severity_string == "alarm"sv)
		return alarmSeverity::alarm;

	if (severity_string == "alert"sv)
		return alarmSeverity::alert;

	throw std::invalid_argument("Passed an invalid severity string, unable to convert");
}

enum class alarmThreshold {
	automaticBasic,
	automaticStateBased,
	ISOLevel, // TODO: Break this down in the the ISO subcategories with constexpr values, with the user determining the category of each machine
	Custom
};

inline std::string toString(const alarmThreshold severity) {
	switch (severity) {
		case alarmThreshold::automaticStateBased:
			return "automaticStateBased";
		case alarmThreshold::ISOLevel:
			return "ISOLevel";
		case alarmThreshold::Custom:
			return "custom";
		default:
			throw std::invalid_argument("Unknown alarm threshold");
	}
}

inline alarmThreshold parse_threshold(const std::string_view threshold_string) {
	if (threshold_string == "automaticStateBased"sv)
		return alarmThreshold::automaticStateBased;

	if (threshold_string == "ISOLevel"sv)
		return alarmThreshold::ISOLevel;

	if (threshold_string == "custom"sv)
		return alarmThreshold::Custom;

	throw std::invalid_argument("Unknown threshold type"); // TODO: Implement a rational noexcept policy.
}


struct alarm_settings_t {
	int channel_id;
	int type_id;
	alarmSeverity severity;

	alarmThreshold threshold;
	std::optional<float> customLevel; // nullopt indicates the threshold type is not custom.

	alarm_settings_t(int channel_id, int type_id, alarmSeverity severity) : channel_id(channel_id), type_id(type_id), severity(severity), threshold(alarmThreshold::automaticStateBased) {}

	alarm_settings_t(int channel_id, int type_id, alarmSeverity severity, alarmThreshold threshold) : channel_id(channel_id), type_id(type_id), severity(severity), threshold(threshold), customLevel(std::nullopt) {}

	alarm_settings_t(int channel_id, int type_id, alarmSeverity severity, alarmThreshold threshold, std::optional<float> custom_level) : channel_id(channel_id), type_id(type_id), severity(severity), threshold(threshold),
																																		 customLevel(custom_level) {}
};

struct alarm_level_history_point {
	time_point_t time_changed;
	float level;

	alarm_level_history_point(const time_point_t& time_changed, float level) : time_changed(time_changed), level(level) {}
};

struct alarm_state_t {
	std::optional<alarmSeverity> severity; // nullopt indicates no alarms are active
	alarm_state_t(std::optional<alarmSeverity>&& severity) : severity(severity) {}

	bool operator==(const alarm_state_t& rhs) const {
		if (severity.has_value() != rhs.severity.has_value())
			return false;
		if (severity == std::nullopt && rhs.severity == std::nullopt)
			return true;

		return severity.value() == rhs.severity.value();
	}

	bool operator!=(const alarm_state_t& rhs) const {
		return !(rhs == *this);
	}
};

struct alarm_activation_t {
	int channel_id;
	int type_id;
	alarmSeverity severity;

	time_point_t activation_time;
	bool became_active;

	alarm_activation_t(int channel_id, int type_id, alarmSeverity severity, const time_point_t& activation_time, bool became_active) : channel_id(channel_id), type_id(type_id), severity(severity), activation_time(activation_time),
																																	   became_active(became_active) {}

	alarm_activation_t(const alarm_settings_t& settings, const time_point_t& activation_time, const alarm_state_t& new_state)
		: activation_time(activation_time), channel_id(settings.channel_id), type_id(settings.type_id) {
		became_active = new_state.severity.has_value();
		severity = settings.severity;
	}
};

struct statistics_point_t {
	float mean = 0;
	float variance = 0;
	time_point_t time_of_last_point_in_calculation = time_point_t(0s);
	double m2 = 0;
	int sample_count = 0;

	statistics_point_t() = default;
	statistics_point_t(float mean, float standard_deviation, const time_point_t& time_of_last_point_in_calculation, double m_2, double sample_count) : mean(mean), variance(standard_deviation),
																																					   time_of_last_point_in_calculation(time_of_last_point_in_calculation), m2(m_2), sample_count(sample_count) {}
};