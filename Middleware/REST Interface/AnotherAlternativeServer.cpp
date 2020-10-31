#include "AnotherAlternativeServer.h"
#include "doctest/doctest.h"

void AnotherAlternativeServer::runServer(const std::string& address) {
	crow::SimpleApp app;

	CROW_ROUTE(app, "/")([]()
						 {
							 return "Hello world";
						 });

	app.port(1234).multithreaded().run();
}

TEST_CASE ("Yet another HTTP server") {
	AnotherAlternativeServer server;
	//server.runServer("http://127.0.0.1:1234");
}