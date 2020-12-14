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
	automatic,
	ISOLevel, // TODO: Break this down in the the ISO subcategories with constexpr values, with the user determining the category of each machine
	Custom
};

inline std::string toString(const alarmThreshold severity) {
	switch (severity) {
		case alarmThreshold::automatic:
			return "automatic";
		case alarmThreshold::ISOLevel:
			return "ISOLevel";
		case alarmThreshold::Custom:
			return "custom";
		default:
			throw std::invalid_argument("Unknown alarm threshold");
	}
}

inline alarmThreshold parse_threshold(const std::string_view threshold_string) {
	if (threshold_string == "automatic"sv)
		return alarmThreshold::automatic;

	if (threshold_string == "ISOLevel"sv)
		return alarmThreshold::ISOLevel;

	if (threshold_string == "custom"sv)
		return alarmThreshold::Custom;

	throw std::invalid_argument("Unknown threshold type"); // TODO: Implement a rational noexcept policy.
}


struct alarm_settings_t {
	int state_id; // TODO: This needs extracting into a typedef somewhere, along with most other IDs
	int channel_id;
	int type_id;
	alarmSeverity severity;

	alarmThreshold threshold;
	std::optional<float> customLevel; // nullopt indicates the threshold type is not custom.

	alarm_settings_t(int state_id, int channel_id, int type_id, alarmSeverity severity, alarmThreshold threshold) : state_id(state_id), channel_id(channel_id), type_id(type_id), severity(severity), threshold(threshold), customLevel(std::nullopt) {}

	alarm_settings_t(int state_id, int channel_id, int type_id, alarmSeverity severity, alarmThreshold threshold, float custom_level) : state_id(state_id), channel_id(channel_id), type_id(type_id), severity(severity), threshold(threshold),
																																		customLevel(custom_level) {}
};

struct automatic_alarm_level_history_point_t {
	int state_id;
	int channel_id;
	int type_id;

	time_point_t time_became_active;
	float level;
};

struct alarm_activation_t {
	int state_id;
	int channel_id;
	int type_id;

	time_point_t activation_time;
	alarmSeverity severity;
	bool acknowledged;
};

struct alarm_state_t {
	std::optional<alarmSeverity> severity; // nullopt indicates no alarms are active
};