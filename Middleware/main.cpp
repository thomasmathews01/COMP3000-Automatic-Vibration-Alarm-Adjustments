#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest/doctest.h"
#include "Database/Database.h"
#include "REST Interface/Server.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace std::string_literals;

int main(int argc, char** argv) {
	doctest::Context context;
	context.applyCommandLine(argc, argv);
	context.setOption("no-breaks", true);             // don't break in the debugger when assertions fail
	int res = context.run(); // run

	if (context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
		return res;          // propagate the result of the tests

	auto log_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logfile");
	auto server_logger = std::make_shared<spdlog::logger>("REST-SERVER", log_file_sink);
	server_logger->set_level(spdlog::level::info);
	spdlog::register_logger(server_logger);


	/*std::shared_ptr<Database> db = std::make_shared<Database>();
	db->set_up_database_connection();
	Server server(db, logger2);
	server.startServer();

	char x;
	std::cin >> x;*/

	return 0;
}