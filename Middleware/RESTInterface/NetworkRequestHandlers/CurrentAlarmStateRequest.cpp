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

crow::response CurrentAlarmStateRequest::get_current_alarm_state(const crow::request& request, const std::shared_ptr<IConfigurationAccess>& config_storage,
                                                              const std::shared_ptr<IAlarmStorage>& alarm_storage) {
	std::vector<int> machine_ids;
	
	if (const auto machine_id = CrowExtractionHelpers::extract_int_from_url_params(request, "machine_id"); machine_id)
		machine_ids.push_back(*machine_id);

	const auto channel_id = CrowExtractionHelpers::extract_int_from_url_params(request, "channel_id");
	if (channel_id)
		machine_ids.push_back(config_storage->get_machine_id_from_channel_id(*channel_id));

	if (const auto site_id = CrowExtractionHelpers::extract_int_from_url_params(request, "site_id"); site_id) {
		const auto machines = config_storage->get_machines_for_site(site(*site_id));
		std::transform(machines.cbegin(), machines.cend(), std::back_inserter(machine_ids), [](const machine& machine) { return machine.id; });
	} // TODO: Unit test cases for the various possibilities of inputs

	if (machine_ids.empty())
		return crow::response(401);

	// Extract optional filters
	const auto type_id = CrowExtractionHelpers::extract_int_from_url_params(request, "type_id");
	std::vector<alarm_activation_t> activations;
	for (const auto machine_id : machine_ids | views::unique)
		for (const auto& activation : alarm_storage->get_activations_for_machine(machine_id))
			activations.push_back(activation);

	const auto severities_of_active_alarms = activations
									   | views::filter([&](const auto& activation) { return !channel_id || activation.channel_id == *channel_id; })
									   | views::filter([&](const auto& activation) { return !type_id || activation.type_id == *type_id; })
									   | views::transform(&alarm_activation_t::severity)
									   | views::unique
									   | to<std::vector>();

	for (const auto type : {alarmSeverity::alarm, alarmSeverity::alert})
		if (ranges::contains(severities_of_active_alarms, type))
			return get_alarm_severity_as_json(type);

		// TODO: This is actually BS, because we have added the ability for the alarms to issue activations with a false param on deactiviation, so if the last thing was a deactivation this will currently report the current state inccorectly.

	return crow::response(get_alarm_severity_as_json(alarmSeverity::none));
}
