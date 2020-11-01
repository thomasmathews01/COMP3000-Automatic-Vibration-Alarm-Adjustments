#pragma once

#include "crow.h"
#include "../Database/IDatabase.h"
#include <thread>
#include <utility>

constexpr auto default_port = 1234;

class Server {
public:
	explicit Server(std::shared_ptr<IDatabase> database) : database(std::move(database)) {}

	~Server();
	void startServer(const std::string& address);

private:
	std::shared_ptr<IDatabase> database;
	std::thread worker;
	bool started = false;
	void work();
};


