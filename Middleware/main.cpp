#include "Database.h"
#include "DatabaseFactory.h"
#include <memory>
#include <iostream>
#include <Hypodermic/Hypodermic.h>
#include <StateStorage.h>
#include <AlarmStorage.h>
#include <ConfigStorage.h>
#include <DataStorage.h>
#include "Server.h"

namespace
{
	std::shared_ptr<Hypodermic::Container> get_container() {
		Hypodermic::ContainerBuilder builder;

		builder.registerType<DatabaseFactory>().as<IDatabaseFactory>().singleInstance();
		builder.registerType<ConfigStorage>().as<IConfigurationAccess>().singleInstance();
		builder.registerType<DataStorage>().as<IDataAccess>().singleInstance();
		builder.registerType<AlarmStorage>().as<IAlarmStorage>().singleInstance();
		builder.registerType<StateStorage>().as<IStateStorage>().singleInstance();
		builder.registerType<Server>().as<IServer>().singleInstance();

		return builder.build();
	}
}

int main(int argc, char** argv) {
	const auto container = get_container();
	const auto server = container->resolve<IServer>();

	server->startServer();
	char x;
	std::cin >> x;
	return 0;
}