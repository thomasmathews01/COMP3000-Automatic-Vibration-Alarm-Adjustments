#include "GetStatePeriodsRequest.h"
#include "../../Database/TestDatabase.h"
#include "../Conversions.h"
#include <doctest/doctest.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

using namespace std::chrono_literals;
using namespace std::chrono;

using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;

std::string get_state_periods_as_json(const std::vector<state_period_t>& states) {
	StringBuffer buffer;
	PrettyWriter writer(buffer);
	writer.StartObject();

	writer.Key("states");
	writer.StartArray();

	for (const auto& state : states) {
		writer.StartObject();
		writer.Key("id");
		writer.Int(state.state_id);
		writer.Key("start_seconds_since_epoch");
		writer.Int(duration_cast<seconds>(state.start.time_since_epoch()).count());
		writer.Key("end_seconds_since_epoch");
		writer.Int(duration_cast<seconds>(state.end.time_since_epoch()).count());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return buffer.GetString();
}

std::string GetStatePeriodsRequest::get_state_periods(const crow::request& request, const std::shared_ptr<IDatabase>& database) {
	// We allow requests by channel, because that it is likely what is being displayed, but in reality they are specified at machine level, so we translate.
	const auto channel_id = std::stoi(request.url_params.get("channel"));
	const auto machine_id = database->get_machine_id_from_channel_id(channel_id);

	auto state_changes = database->get_state_changes_for_machine(machine_id);
	const auto state_periods = Conversions::convert_state_changes_to_state_periods(std::move(state_changes));

	return get_state_periods_as_json(state_periods);
}
