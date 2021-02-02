#pragma once

#include "IDatabase.h"
#include "IServer.h"
#include <thread>
#include <utility>

constexpr auto default_port = 1234;

class Server : public IServer {
public:
	explicit Server(std::shared_ptr<IDatabase> database) : database(std::move(database)) {}

	~Server() override;
	void startServer() final;

private:
	std::shared_ptr<IDatabase> database;
	std::shared_ptr<IConfigurationAccess> configuration_storage;
	std::shared_ptr<IStateStorage> state_storage;
	std::shared_ptr<IDataAccess> data_storage;
	std::shared_ptr<IAlarmStorage> alarm_storage;
	std::thread worker;
	bool started = false;
	void work();
};


