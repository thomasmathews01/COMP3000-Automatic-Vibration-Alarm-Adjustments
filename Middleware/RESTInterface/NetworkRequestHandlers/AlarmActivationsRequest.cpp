#include "AlarmActivationsRequest.h"
#include "../../Utils/STLExtensions.h"
#include "../Utils/CrowExtractionHelpers.h"
#include <range/v3/view/filter.hpp>
#include <range/v3/core.hpp>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <chrono>
using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;
using namespace std::string_literals;
using namespace ranges;
using namespace std::chrono;
using namespace std::chrono_literals;

std::string activations_as_json_string(const std::vector<alarm_activation_t>& activations) {
	StringBuffer buff;
	PrettyWriter<StringBuffer> writer(buff);
	writer.StartObject();

	writer.Key("activations");
	writer.StartArray();

	for (const auto& activation : activations) {
		writer.StartObject();
		writer.Key("time");
		writer.Int(duration_cast<seconds>(activation.activation_time.time_since_epoch()).count());
		writer.Key("channel_id");
		writer.Double(activation.channel_id);
		writer.Key("state_id");
		writer.Double(activation.state_id);
		writer.Key("type_id");
		writer.Double(activation.type_id);
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return buff.GetString();
}

std::string AlarmActivationsRequest::get_activations(const crow::request& request, alarmSeverity severity, std::shared_ptr<IDatabase>& database) {
	// Extract identifying information
	auto machine_id = CrowExtractionHelpers::extract_int_from_url_params(request, "machine_id");
	const auto channel_id = CrowExtractionHelpers::extract_int_from_url_params(request, "channel_id");
	machine_id = machine_id ? machine_id : database->get_machine_id_from_channel_id(*channel_id);
	if (!machine_id)
		return ""; // TODO: Error handling for invalid identifiers case

	// Extract optional filters
	const auto type_id = CrowExtractionHelpers::extract_int_from_url_params(request, "type_id");
	const auto state_id = CrowExtractionHelpers::extract_int_from_url_params(request, "state_id");

	const auto activations = database->get_activations_for_machine(*machine_id);

	return activations_as_json_string(activations
									  | views::filter([&](const auto& activation) { return activation.severity == severity; })
									  | views::filter([&](const auto& activation) { return !channel_id || activation.channel_id == *channel_id; })
									  | views::filter([&](const auto& activation) { return !type_id || activation.type_id == *type_id; })
									  | views::filter([&](const auto& activation) { return !state_id || activation.state_id == *state_id; })
									  | to<std::vector>());
}
