#pragma once

#include "IDatabase.h"
#include "IServer.h"
#include <thread>
#include <utility>

constexpr auto default_port = 1234;

class Server : public IServer {
public:
	explicit Server(std::shared_ptr<IDatabase> database) : database(std::move(database)) {}

	~Server();
	void startServer() final;

private:
	std::shared_ptr<IDatabase> database;
	std::thread worker;
	bool started = false;
	void work();
};


