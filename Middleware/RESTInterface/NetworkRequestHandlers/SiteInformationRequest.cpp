#include <rapidjson/prettywriter.h>
#include "SiteInformationRequest.h"
#include "../../Database/TestDatabase.h"


std::string SiteInformationRequest::get_site_information(const crow::request& request, const std::shared_ptr<IDatabase>& database) {
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter writer(buffer);
	writer.StartObject();

	const auto queries = request.url_params;
	const auto site_id = std::stoi(queries.get("site"));

	site s(site_id);
	database->get_machines_for_site(s);

	writer.Key("machines");
	writer.StartArray();

	for (const auto& machine : s.machines) {
		writer.StartObject();
		writer.Key("id");
		writer.Int(machine.id);
		writer.Key("name");
		writer.String(machine.name.c_str());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return buffer.GetString();
}

/*

TEST_CASE ("Return machine information") {
	std::shared_ptr<TestDatabase> test_db = std::make_shared<TestDatabase>();
	test_db->machines.emplace_back(1, "firstMachineName");
	test_db->machines.emplace_back(2, "secondMachineName");
	test_db->machines.emplace_back(3, "thirdMachineName");

	auto exampleRequest = crow::request();
	const auto params = crow::query_string("http://localhost:3000/site?site=1");
	exampleRequest.url_params = params;

	const auto result = SiteInformationRequest::get_site_information(exampleRequest, test_db);

		CHECK_NE(result.find("firstMachineName"), std::string::npos);
		CHECK_NE(result.find("secondMachineName"), std::string::npos);
		CHECK_NE(result.find("thirdMachineName"), std::string::npos);
		CHECK_NE(result.find('1'), std::string::npos);
		CHECK_NE(result.find('2'), std::string::npos);
		CHECK_NE(result.find('3'), std::string::npos);
}*/
