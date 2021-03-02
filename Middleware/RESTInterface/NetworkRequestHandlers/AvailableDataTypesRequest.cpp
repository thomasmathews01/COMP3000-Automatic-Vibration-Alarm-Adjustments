#include "AvailableDataTypesRequest.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include "../Utils/CrowUtils.h"

using rapidjson::PrettyWriter;
using rapidjson::StringBuffer;

std::string get_json_for_types_list(const std::vector<std::pair<int, std::string>>& types) {
	StringBuffer buff;
	PrettyWriter<StringBuffer> writer(buff);
	writer.StartObject();

	writer.Key("types");
	writer.StartArray();

	for (const auto& type : types) {
		writer.StartObject();
		writer.Key("id");
		writer.Double(type.first);
		writer.Key("name");
		writer.String(type.second.c_str());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return buff.GetString();
}

crow::response AvailableDataTypesRequest::get_available_data_types(const crow::request& request, const std::shared_ptr<IDataAccess>& database) {
    const auto types = database->get_data_types_available_for_channel(0); // TODO: Currently the code just ignores the channel, but it also needs refactoring with a new signature.

    return CrowUtils::add_cors_headers(get_json_for_types_list(types));
}
