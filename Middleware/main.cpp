#include <DatabaseFactory.h>
#include <memory>
#include <iostream>
#include <Hypodermic/Hypodermic.h>
#include <StateStorage.h>
#include <AlarmStorage.h>
#include <ConfigStorage.h>
#include <DataStorage.h>
#include <RealClock.h>
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
		builder.registerType<RealClock>().as<IClock>().singleInstance();
		builder.registerType<Server>().as<IServer>().singleInstance();

		return builder.build();
	}
}

int main(int argc, char** argv) {
	const auto container = get_container();
	const auto something = container->resolve<IDatabaseFactory>();
	const auto something2 = container->resolve<IConfigurationAccess>();
	const auto something3 = container->resolve<IDataAccess>();
	const auto something4 = container->resolve<IAlarmStorage>();
	const auto something5 = container->resolve<IStateStorage>();
	const auto something6 = container->resolve<IClock>();
	auto server = container->resolve<IServer>();

	server->startServer();
	char x;
	std::cin >> x;
	return 0;
}