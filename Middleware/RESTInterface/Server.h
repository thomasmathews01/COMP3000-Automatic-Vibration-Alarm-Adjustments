#pragma once

#include "IDatabase.h"
#include "IServer.h"
#include <thread>
#include <utility>
#include <IClock.h>

constexpr auto default_port = 1234;

class Server : public IServer {
public:
	explicit Server(std::shared_ptr<IConfigurationAccess> configuration_storage, std::shared_ptr<IStateStorage> state_storage, std::shared_ptr<IDataAccess> data_storage, std::shared_ptr<IAlarmStorage> alarm_storage,
					std::shared_ptr<IClock> clock);

	~Server() override;
	void startServer() noexcept final;

private:
	const std::shared_ptr<IConfigurationAccess> configuration_storage;
	const std::shared_ptr<IStateStorage> state_storage;
	const std::shared_ptr<IDataAccess> data_storage;
	const std::shared_ptr<IAlarmStorage> alarm_storage;
	const std::shared_ptr<IClock> clock;

	std::thread worker;
	bool started = false;
	void work() const;
};


