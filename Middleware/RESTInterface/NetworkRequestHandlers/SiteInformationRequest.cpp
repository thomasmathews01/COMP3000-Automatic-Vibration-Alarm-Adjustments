#include <rapidjson/prettywriter.h>
#include "SiteInformationRequest.h"
#include "../../Database/TestDatabase.h"
#include "../Utils/CrowUtils.h"

crow::response SiteInformationRequest::get_site_information(const crow::request& request, const std::shared_ptr<IConfigurationAccess>& database) {
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter writer(buffer);
	writer.StartObject();

	const auto queries = request.url_params;
	const auto site_id = std::stoi(queries.get("site"));

	site s(site_id);
	const auto machines = database->get_machines_for_site(s);

	writer.Key("machines");
	writer.StartArray();

	for (const auto& machine : machines) {
		writer.StartObject();
		writer.Key("id");
		writer.Int(machine.id);
		writer.Key("name");
		writer.String(machine.name.c_str());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return CrowUtils::add_cors_headers(crow::response(buffer.GetString()));
}