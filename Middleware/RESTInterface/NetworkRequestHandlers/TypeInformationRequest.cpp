#include "TypeInformationRequest.h"
#include "MachineInformationRequest.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <range/v3/algorithm/find_if.hpp>
#include "../Utils/CrowUtils.h"
#include "range/v3/algorithm/find.hpp"

using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;

crow::response TypeInformationRequest::get_type_information(const crow::request& request, const std::shared_ptr<IConfigurationAccess>& database) {
	const auto type_id = std::stoi(request.url_params.get("type_id"));
	const auto types = database->get_all_types();
	const auto type = ranges::find(types, type_id, &data_type::id);
	if (type == ranges::end(types))
		return crow::response(500); // TODO: Better

	StringBuffer buffer;
	PrettyWriter writer(buffer);
	writer.StartObject();

	writer.Key("id");
	writer.Int(type->id);
	writer.Key("name");
	writer.String(type->name.c_str());

	writer.EndObject();

	return CrowUtils::add_cors_headers(crow::response(buffer.GetString()));
}
