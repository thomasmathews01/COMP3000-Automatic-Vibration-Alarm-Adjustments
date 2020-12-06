#include "Server.h"
#include "crow.h"
#include "NetworkRequestHandlers/HelloWorldRequest.h"
#include "NetworkRequestHandlers/GetSitesRequest.h"
#include "NetworkRequestHandlers/SiteInformationRequest.h"
#include "NetworkRequestHandlers/MachineInformationRequest.h"
#include "NetworkRequestHandlers/CurrentTimeRequest.h"
#include "NetworkRequestHandlers/AvailableDataTypesRequest.h"
#include "NetworkRequestHandlers/GetDataRequest.h"
#include "NetworkRequestHandlers/GetStatePeriodsRequest.h"

using namespace std::string_literals;

Server::~Server() {
	if (started) {
		started = false;
		worker.join(); // TODO: This will never actually join because there is no cooperative cancellation, to be implemented later.
	}
}

crow::response pre_flight_response(const crow::request& req) {
	crow::response response(200);

	response.add_header("Access-Control-Allow-Origin", "*");
	response.add_header("Access-Control-Allow-Headers", "*");
	response.add_header("Access-Control-Allow-Methods", "*");
	response.add_header("Access-Control-Allow-Credentials", "true");

	return response;
}

crow::response handle_login(const crow::request& req) {
	if (req.method == crow::HTTPMethod::OPTIONS)
		return pre_flight_response(req);

	const auto username = req.url_params.get("username");
	const auto password = req.url_params.get("password");

	crow::response response;
	if (username == "admin"s && password == "admin"s)
		return crow::response(200, "Accepted");
	else return crow::response(401, "Rejected");
}

void Server::work() {
	crow::SimpleApp app;

	CROW_ROUTE(app, "/")([]() { return HelloWorldRequest::get_hello_world_response(); });
	CROW_ROUTE(app, "/sites")([this]() { return GetSitesRequest::get_sites_info(this->database); });
	CROW_ROUTE(app, "/site")([this](const crow::request& request) { return SiteInformationRequest::get_site_information(request, this->database); });
	CROW_ROUTE(app, "/machine")([this](const crow::request& request) { return MachineInformationRequest::get_machine_information(request, this->database); });
	CROW_ROUTE(app, "/time")([]() { return CurrentTimeRequest::get_current_time_response(); });
	CROW_ROUTE(app, "/types")([this](const crow::request& req) { return AvailableDataTypesRequest::get_available_data_types(req, this->database); });
	CROW_ROUTE(app, "/data")([this](const crow::request& req) { return GetDataRequest::get_data_points(req, this->database); });
	CROW_ROUTE(app, "/states")([this](const crow::request& req) { return GetStatePeriodsRequest::get_state_periods(req, this->database); });
	//CROW_ROUTE(app, "/alarms")([this](const crow::request& req) { return GetDataRequest::get_data_points(req, this->database); });
	//«CROW_ROUTE(app, "/alerts")([this](const crow::request& req) { return GetDataRequest::get_data_points(req, this->database); });


	CROW_ROUTE(app, "/login").methods("POST"_method, "OPTIONS"_method)([](const crow::request& req) { return handle_login(req); });

	app.port(default_port).multithreaded().run();
}

void Server::startServer() {
	if (!started) {
		worker = std::thread([this]() { this->work(); });
		started = true;
	}
}
