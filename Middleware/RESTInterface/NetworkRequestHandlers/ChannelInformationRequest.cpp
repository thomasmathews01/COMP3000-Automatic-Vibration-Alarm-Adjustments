#include "ChannelInformationRequest.h"
#include "MachineInformationRequest.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include "../Utils/CrowUtils.h"
#include "range/v3/algorithm/find.hpp"

using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;

crow::response ChannelInformationRequest::get_channel_information(const crow::request& request, const std::shared_ptr<IConfigurationAccess>& database) {
	const auto channel_id = std::stoi(request.url_params.get("channel_id"));
	const auto channels = database->get_all_channels();
	const auto channel = ranges::find(channels, channel_id, &channel::id);
	if (channel == ranges::end(channels))
		return crow::response(500); // TODO: Better

	StringBuffer buffer;
	PrettyWriter writer(buffer);
	writer.StartObject();

	writer.Key("id");
	writer.Int(channel->id);
	writer.Key("name");
	writer.String(channel->name.c_str());

	writer.EndObject();

	return CrowUtils::add_cors_headers(crow::response(buffer.GetString()));
}
