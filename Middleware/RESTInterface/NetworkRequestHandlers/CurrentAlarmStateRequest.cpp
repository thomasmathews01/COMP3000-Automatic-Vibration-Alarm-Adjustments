#include "CurrentAlarmStateRequest.h"
#include "../../Utils/STLExtensions.h"
#include <range/v3/view/unique.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/find.hpp>
#include "../Utils/CrowExtractionHelpers.h"

using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;
using namespace ranges;

std::string get_alarm_severity_as_json(const alarmSeverity severity) {
	StringBuffer buff;
	PrettyWriter<StringBuffer> writer(buff);
	writer.StartObject();

	writer.Key("state");
	writer.String(toString(severity).c_str());
	writer.EndObject();

	return buff.GetString();
}

std::string CurrentAlarmStateRequest::get_current_alarm_state(const crow::request& request, alarmSeverity severity, std::shared_ptr<IConfigurationAccess>& config_storage,
                                                              std::shared_ptr<IAlarmStorage>& alarm_storage) { // TODO: We
    // should
    // support sites, but this requires a refactoring to get rid of the nonsense API to the alarm_storage structure
	// Extract identifying information
	auto machine_id = CrowExtractionHelpers::extract_int_from_url_params(request, "machine_id");
	const auto channel_id = CrowExtractionHelpers::extract_int_from_url_params(request, "channel_id");
	machine_id = machine_id ? machine_id : config_storage->get_machine_id_from_channel_id(*channel_id);

	if (!machine_id)
		return ""; // TODO: Error handling for invalid identifiers case

	// Extract optional filters
	const auto type_id = CrowExtractionHelpers::extract_int_from_url_params(request, "type_id");

	const auto all_activations = alarm_storage->get_activations_for_machine(*machine_id);
	const auto severities_of_active_alarms = all_activations
									   | views::filter([&](const auto& activation) { return activation.severity == severity; })
									   | views::filter([&](const auto& activation) { return !channel_id || activation.channel_id == *channel_id; })
									   | views::filter([&](const auto& activation) { return !type_id || activation.type_id == *type_id; })
									   | views::transform([](const auto& activation) { return activation.severity; })
									   | views::unique
									   | to<std::vector>();

	for (const auto type : {alarmSeverity::alarm, alarmSeverity::alert})
		if (ranges::find(severities_of_active_alarms, type) != ranges::end(severities_of_active_alarms))
			return get_alarm_severity_as_json(type);

	return get_alarm_severity_as_json(alarmSeverity::none);
}
