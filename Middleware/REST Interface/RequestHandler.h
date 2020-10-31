#pragma once

#include <cpprest/http_msg.h>
#include <cpprest/json.h>

using namespace web::http;


class RequestHandler {
public:
	std::pair<status_code, web::json::value> HandleRequest(const http_request& request);
private:
	std::pair<status_code, web::json::value> HandleGETRequest(const http_request& request);
	std::pair<status_code, web::json::value> HandlePOSTRequest(const http_request& request);
	std::pair<status_code, web::json::value> HandlePUTRequest(const http_request& request);
	std::pair<status_code, web::json::value> HandleDELETERequest(const http_request& request);
};


