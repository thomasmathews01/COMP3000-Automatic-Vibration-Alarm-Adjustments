#include "MachineInformationRequest.h"
#include "../../Database/TestDatabase.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <doctest/doctest.h>


using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;

std::string MachineInformationRequest::get_machine_information(const crow::request& request, const std::shared_ptr<IDatabase>& database) {
	StringBuffer buffer;
	PrettyWriter writer(buffer);
	writer.StartObject();

	const auto machine_id = std::stoi(request.url_params.get("machine"));
	machine m(machine_id);
	database->populate_channel_information_for_a_machine(m);

	writer.Key("channels");
	writer.StartArray();

	for (const auto& channel : m.channels) {
		writer.StartObject();
		writer.Key("id");
		writer.Int(channel.id);
		writer.Key("name");
		writer.String(channel.name.c_str());
		writer.Key("name");
		writer.String(channel.units.c_str());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return buffer.GetString();
}

TEST_CASE ("Return channel information") {
	std::shared_ptr<TestDatabase> test_db = std::make_shared<TestDatabase>();
	test_db->channels.emplace_back(1, "firstChannelName", "g");
	test_db->channels.emplace_back(2, "secondChannelName", "ips");
	test_db->channels.emplace_back(3, "thirdChannelName", "mil");

	auto request = crow::request();
	const auto params = crow::query_string("http://localhost:3001/machine?machine=1");
	request.url_params = params;

	const auto result = MachineInformationRequest::get_machine_information(request, test_db);

		CHECK_NE(result.find("firstChannelName"), std::string::npos);
		CHECK_NE(result.find("secondChannelName"), std::string::npos);
		CHECK_NE(result.find("thirdChannelName"), std::string::npos);
		CHECK_NE(result.find("\"g\""), std::string::npos);
		CHECK_NE(result.find("\"ips\""), std::string::npos);
		CHECK_NE(result.find("\"mil\""), std::string::npos);
		CHECK_NE(result.find('1'), std::string::npos);
		CHECK_NE(result.find('2'), std::string::npos);
		CHECK_NE(result.find('3'), std::string::npos);
}
