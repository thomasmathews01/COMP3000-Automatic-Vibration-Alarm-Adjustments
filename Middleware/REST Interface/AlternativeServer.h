#include <cpprest/http_listener.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

#include <map>
#include <string>
#include "RequestHandler.h"

class Server {
public:
	explicit Server(const RequestHandler&& request_handler) : handler(request_handler) {}

	void startServer(const std::string& address);
	void stopServer();
private:
	void handle_get(const http_request& request);
	static void display_json(const json::value& jvalue, const std::string& prefix);
	void handle_request(const http_request& request, std::function<void(const json::value&, json::value&)> action);

	std::map<std::string, std::string> dictionary;
	http_listener listener;
	RequestHandler handler;
};