#include "AvailableDataTypesRequest.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

using rapidjson::PrettyWriter;
using rapidjson::StringBuffer;

std::string get_json_for_types_list(const std::vector<std::pair<int, std::string>>& types) {
	StringBuffer buff;
	PrettyWriter<StringBuffer> writer(buff);
	writer.StartObject();

	writer.Key("data_types");
	writer.StartArray();

	for (const auto& type : types) {
		writer.StartObject();
		writer.Key("id");
		writer.Double(type.first);
		writer.Key("displayName");
		writer.String(type.second.c_str());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return buff.GetString();
}

std::string AvailableDataTypesRequest::get_available_data_types(const crow::request& request, const std::shared_ptr<IDataAccess>& database) {
    const auto channel_id = std::stoi(request.url_params.get("channel"));

    const auto types = database->get_data_types_available_for_channel(channel_id);

    return get_json_for_types_list(types);
}
