#include "AlternativeServer.h"
#include "doctest/doctest.h"
#include <chrono>
#include <thread>

using namespace std::string_literals;

void Server::handle_get(const http_request& request) {
	const auto handle_result = handler.HandleRequest(request);
	std::cout << "Handling GET request" << std::endl;

	request.reply(handle_result.first, handle_result.second);
}

void Server::startServer(const std::string& address) {
	listener = http_listener(address);

	listener.support(methods::GET, [this](auto&& PH1) { handle_get(std::forward<decltype(PH1)>(PH1)); }); // Clang-tidy reckons this is better than a normal std bind...

	try {
		listener.open().wait();
		std::cout << "Server Started" << std::endl;
	}
	catch (std::exception const& e) {
		std::cout << "Exception thrown whilst starting server to handle requests. Server shall die gracefully. Error: " << e.what();
	}
}

void Server::stopServer() {
	std::cout << "Received stop request.." << std::endl;
	listener.close().wait();
	std::cout << "Stopped" << std::endl;
}