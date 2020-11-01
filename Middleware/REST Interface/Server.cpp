#include "Server.h"
#include "doctest/doctest.h"
#include "../Database/Database.h"
#include "../Database/TestDatabase.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

using namespace std::string_literals;
using namespace rapidjson;

Server::~Server() {
	if (started) {
		started = false;
		worker.join(); // TODO: This will never actually join because there is no cooperative cancellation, to be implemented later.
	}
}

std::string handle_hello_world() {
	return "Hello World"s;
}

std::string handle_get_sites(const std::shared_ptr<IDatabase>& database) {
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

std::string handle_get_site_info(const std::shared_ptr<IDatabase>& database, const int site_id) {
	StringBuffer buffer;
	PrettyWriter writer(buffer);
	writer.StartObject();

	site s(site_id);
	database->populate_sites_machine_information(s);

	writer.Key("machines");
	writer.StartArray();

	for (const auto& machine : s.machines) {
		writer.Key("id");
		writer.Int(machine.id);
		writer.StartObject();
		writer.Key("name");
		writer.String(machine.name.c_str());
		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return buffer.GetString();
}

std::string handle_get_machine_info(const std::shared_ptr<IDatabase>& database, int machine_id) {
	StringBuffer buffer;
	PrettyWriter writer(buffer);
	writer.StartObject();

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

void Server::work() {
	crow::SimpleApp app;

	CROW_ROUTE(app, "/")([]() { return handle_hello_world(); });
	CROW_ROUTE(app, "/sites")([this]() { return handle_get_sites(this->database); });
	CROW_ROUTE(app, "/site/id/<int>")([this](const int site_id) { return handle_get_site_info(this->database, site_id); });
	CROW_ROUTE(app, "/machine/id/<int>")([this](const int machine_id) { return handle_get_machine_info(this->database, machine_id); });

	app.port(default_port).multithreaded().run();
}

void Server::startServer(const std::string& address) {
	if (!started) {
		worker = std::thread([this]() { this->work(); });
		started = true;
	}
}

TEST_CASE ("Return site information") {
	std::shared_ptr<TestDatabase> test_db = std::make_shared<TestDatabase>();
	test_db->sites.emplace_back(1, "firstSiteName");
	test_db->sites.emplace_back(2, "secondSiteName");
	test_db->sites.emplace_back(3, "thirdSiteName");

	const auto result = handle_get_sites(test_db);

		CHECK_NE(result.find("firstSiteName"), std::string::npos);
		CHECK_NE(result.find("secondSiteName"), std::string::npos);
		CHECK_NE(result.find("thirdSiteName"), std::string::npos);
		CHECK_NE(result.find('1'), std::string::npos);
		CHECK_NE(result.find('2'), std::string::npos);
		CHECK_NE(result.find('3'), std::string::npos);
}

TEST_CASE ("Return machine information") {
	std::shared_ptr<TestDatabase> test_db = std::make_shared<TestDatabase>();
	test_db->machines.emplace_back(1, "firstMachineName");
	test_db->machines.emplace_back(2, "secondMachineName");
	test_db->machines.emplace_back(3, "thirdMachineName");

	const auto result = handle_get_site_info(test_db, 1);

		CHECK_NE(result.find("firstMachineName"), std::string::npos);
		CHECK_NE(result.find("secondMachineName"), std::string::npos);
		CHECK_NE(result.find("thirdMachineName"), std::string::npos);
		CHECK_NE(result.find('1'), std::string::npos);
		CHECK_NE(result.find('2'), std::string::npos);
		CHECK_NE(result.find('3'), std::string::npos);
}

TEST_CASE ("Return channel information") {
	std::shared_ptr<TestDatabase> test_db = std::make_shared<TestDatabase>();
	test_db->channels.emplace_back(1, "firstChannelName", "g");
	test_db->channels.emplace_back(2, "secondChannelName", "ips");
	test_db->channels.emplace_back(3, "thirdChannelName", "mil");

	const auto result = handle_get_machine_info(test_db, 1);

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