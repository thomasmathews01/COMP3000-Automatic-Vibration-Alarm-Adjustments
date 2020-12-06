#include "GetSitesRequest.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include "doctest/doctest.h"
#include "../../Database/TestDatabase.h"

using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;

std::string GetSitesRequest::get_sites_info(const std::shared_ptr<IDatabase>& database) {
	StringBuffer buff;
	PrettyWriter<StringBuffer> writer(buff);
	writer.StartObject();

	writer.Key("sites");
	writer.StartArray();

	for (const auto& site : database->get_site_data()) {
		writer.StartObject();
		writer.Key("id");
		writer.Int(site.id);
		writer.Key("name");
		writer.String(site.name.c_str());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return buff.GetString();
}

TEST_CASE ("Return site information") {
	std::shared_ptr<TestDatabase> test_db = std::make_shared<TestDatabase>();
	test_db->sites.emplace_back(1, "firstSiteName");
	test_db->sites.emplace_back(2, "secondSiteName");
	test_db->sites.emplace_back(3, "thirdSiteName");

	const auto result = GetSitesRequest::get_sites_info(test_db);

		CHECK_NE(result.find("firstSiteName"), std::string::npos);
		CHECK_NE(result.find("secondSiteName"), std::string::npos);
		CHECK_NE(result.find("thirdSiteName"), std::string::npos);
		CHECK_NE(result.find('1'), std::string::npos);
		CHECK_NE(result.find('2'), std::string::npos);
		CHECK_NE(result.find('3'), std::string::npos);
}