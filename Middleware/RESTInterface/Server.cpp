#include "Server.h"
#include "Utils/include_crow.h"
#include <spdloghelper.h>
#include "NetworkRequestHandlers/all.h"
#include "Utils/CrowExtractionHelpers.h"
#include "Utils/CrowUtils.h"

using namespace std::string_literals;

Server::~Server() {
	if (started) {
		started = false;
		worker.join(); // TODO: This will never actually join because there is no cooperative cancellation, to be implemented later.
	}
}

crow::response pre_flight_response(const crow::request& req) {
	crow::response response(200);

	return CrowUtils::add_cors_headers(std::move(response));
}

crow::response handle_login(const crow::request& req) {
	const auto logger = spdlog::get("REST-SERVER");
	logger->debug("Handling login request. ");

	if (req.method == crow::HTTPMethod::Options)
		return pre_flight_response(req);

	const auto username = CrowExtractionHelpers::extract_string_from_url_params(req, "username");
	const auto password = CrowExtractionHelpers::extract_string_from_url_params(req, "password");

	if (!username || !password)
	{
		logger->error("Login Request was of invalid format, returning error 400");
		return CrowUtils::add_cors_headers(crow::response(400));
	}

	const auto credentials_authorised = *username == "admin"s && *password == "admin"s;

	if (credentials_authorised) {
		logger->debug("Login request was valid, login accepted.");
		return CrowUtils::add_cors_headers(crow::response(200, "Accepted"));
	}

	logger->debug("Login request was not valid, login rejected");
	return CrowUtils::add_cors_headers(crow::response(401, "Rejected"));
}

void Server::work() const {
	const auto logger = spdlog_helpers::get_logger("REST-SERVER");
	logger->info("Running set up...");
	crow::SimpleApp app;

	CROW_ROUTE(app, "/")([]() { return HelloWorldRequest::get_hello_world_response(); });
	CROW_ROUTE(app, "/sites")([this]() { return GetSitesRequest::get_sites_info(configuration_storage); });
	CROW_ROUTE(app, "/machines")([this](const crow::request& request) { return SiteInformationRequest::get_site_information(request, configuration_storage); });
	CROW_ROUTE(app, "/channels")([this](const crow::request& request) { return MachineInformationRequest::get_machine_information(request, configuration_storage); });
	CROW_ROUTE(app, "/time")([this]() { return CurrentTimeRequest::get_current_time_response(clock); });
	CROW_ROUTE(app, "/dataTypes")([this](const crow::request& req) { return AvailableDataTypesRequest::get_available_data_types(req, data_storage); });
	CROW_ROUTE(app, "/data")([this](const crow::request& req) { return GetDataRequest::get_data_points(req, data_storage); });
	CROW_ROUTE(app, "/states")([this](const crow::request& req) { return GetStatePeriodsRequest::get_state_periods(req, configuration_storage, state_storage); });
	CROW_ROUTE(app, "/alarmActivations")([this](const crow::request& req) { return AlarmActivationsRequest::get_activations(req, alarmSeverity::alarm, configuration_storage, alarm_storage); });
	CROW_ROUTE(app, "/alertActivations")([this](const crow::request& req) { return AlarmActivationsRequest::get_activations(req, alarmSeverity::alert, configuration_storage, alarm_storage); });
	CROW_ROUTE(app, "/currentAlarmState")([this](const crow::request& req) { return CurrentAlarmStateRequest::get_current_alarm_state(req, alarmSeverity::alarm,
                                                                                                                                   configuration_storage, alarm_storage); });
	CROW_ROUTE(app, "/currentAlertState")([this](const crow::request& req) { return CurrentAlarmStateRequest::get_current_alarm_state(req, alarmSeverity::alert,
                                                                                                                                   configuration_storage, alarm_storage); });
	CROW_ROUTE(app, "/alarmSettings").methods("POST"_method, "GET"_method)([this](const crow::request& req) { return AlarmSettingsRequest::alarm_settings(req, alarmSeverity::alarm, alarm_storage); });
	CROW_ROUTE(app, "/alertSettings").methods("POST"_method, "GET"_method)([this](const crow::request& req) { return AlarmSettingsRequest::alarm_settings(req, alarmSeverity::alert, alarm_storage); });
	CROW_ROUTE(app, "/alarmLevelHistory")([this](const crow::request& req) { return GetAlarmLevelHistory::get_alarm_level_history(req, alarmSeverity::alarm, alarm_storage); });
	CROW_ROUTE(app, "/alertLevelHistory")([this](const crow::request& req) { return GetAlarmLevelHistory::get_alarm_level_history(req, alarmSeverity::alert, alarm_storage); });


	CROW_ROUTE(app, "/login").methods("POST"_method, "OPTIONS"_method)([](const crow::request& req) { return handle_login(req); });

	app.port(default_port).multithreaded().run();
	logger->info("Sever now running on port: " + std::to_string(default_port));
}

void Server::startServer() noexcept {
	if (!started) {
		worker = std::thread([this]() { this->work(); });
		started = true;
	}
}

Server::Server(std::shared_ptr<IConfigurationAccess> configuration_storage, std::shared_ptr<IStateStorage> state_storage, std::shared_ptr<IDataAccess> data_storage, std::shared_ptr<IAlarmStorage> alarm_storage,
			   const std::shared_ptr<IClock> clock)
	: configuration_storage(std::move(configuration_storage)), state_storage(std::move(state_storage)), data_storage(std::move(data_storage)), alarm_storage(std::move(alarm_storage)), clock(clock) {}
