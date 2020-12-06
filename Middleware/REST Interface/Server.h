#pragma once

#include "../Database/IDatabase.h"
#include <thread>
#include <utility>
#include <spdlog/logger.h>

constexpr auto default_port = 1234;

class Server {
public:
	explicit Server(std::shared_ptr<IDatabase> database, std::shared_ptr<spdlog::logger> ptr) : database(std::move(database)), logger(ptr) {}

	~Server();
	void startServer();

private:
	std::shared_ptr<IDatabase> database;
	std::shared_ptr<spdlog::logger> logger;
	std::thread worker;
	bool started = false;
	void work();
};


