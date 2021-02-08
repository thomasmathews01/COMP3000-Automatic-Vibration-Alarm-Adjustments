#include "MachineInformationRequest.h"
#include "../../Database/TestDatabase.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include "../Utils/CrowUtils.h"

using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;

crow::response MachineInformationRequest::get_machine_information(const crow::request& request, const std::shared_ptr<IConfigurationAccess>& database) {
	StringBuffer buffer;
	PrettyWriter writer(buffer);
	writer.StartObject();

	const auto machine_id = std::stoi(request.url_params.get("machine"));
	machine m(machine_id);
	const auto channels = database->get_channel_information_for_machine(m);

	writer.Key("channels");
	writer.StartArray();

	for (const auto& channel : channels) {
		writer.StartObject();
		writer.Key("id");
		writer.Int(channel.id);
		writer.Key("name");
		writer.String(channel.name.c_str());
		writer.Key("units");
		writer.String(channel.units.c_str());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return CrowUtils::add_cors_headers(crow::response(buffer.GetString()));
}