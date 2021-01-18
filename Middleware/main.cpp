#include "Database.h"
#include "DatabaseFactory.h"
#include <memory>
#include <Hypodermic/Hypodermic.h>
#include "Server.h"

namespace
{
	std::shared_ptr<Hypodermic::Container> get_container() {
		Hypodermic::ContainerBuilder builder;

		builder.registerType<Database>().as<IDatabase>().singleInstance();
		builder.registerType<DatabaseFactory>().as<IDatabaseFactory>().singleInstance();
		builder.registerType<Server>().as<IServer>().singleInstance();

		return builder.build();
	}
}

int main(int argc, char** argv) {
	const auto container = get_container();
	const auto server = container->resolve<IServer>();

	return 0;
}