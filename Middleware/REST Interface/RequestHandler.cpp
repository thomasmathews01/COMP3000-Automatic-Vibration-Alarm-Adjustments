#include "RequestHandler.h"
#include "doctest/doctest.h"

std::pair<status_code, web::json::value> RequestHandler::HandleRequest(const http_request& request) {
	const auto& method_string = request.method();

	if (method_string == "GET")
		return HandleGETRequest(request);
	if (method_string == "POST")
		return HandlePOSTRequest(request);
	if (method_string == "PUT")
		return HandlePUTRequest(request);
	if (method_string == "DELETE")
		return HandleDELETERequest(request);

	return {status_codes::BadRequest, web::json::value::object()};
}

std::pair<status_code, web::json::value> RequestHandler::HandleGETRequest(const http_request& request) {
	return {status_codes::NotImplemented, web::json::value::object()};
}

std::pair<status_code, web::json::value> RequestHandler::HandlePOSTRequest(const http_request& request) {
	return {status_codes::NotImplemented, web::json::value::object()};
}

std::pair<status_code, web::json::value> RequestHandler::HandlePUTRequest(const http_request& request) {
	return {status_codes::NotImplemented, web::json::value::object()};
}

std::pair<status_code, web::json::value> RequestHandler::HandleDELETERequest(const http_request& request) {
	return {status_codes::NotImplemented, web::json::value::object()};
}

TEST_CASE ("Can respond to GET request") {
	RequestHandler handler;
	http_request request;
	request.set_method(methods::GET);
	const auto response = handler.HandleRequest(request);

		CHECK_EQ(status_codes::NotImplemented, response.first);
}