#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest/doctest.h"
#include "REST Interface/RequestHandler.h"
#include "REST Interface/AlternativeServer.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace std::string_literals;

int main(int argc, char** argv) {
	doctest::Context context;

	// defaults
	context.addFilter("test-case-exclude", "*math*"); // exclude test cases with "math" in their name
	context.setOption("abort-after", 5);              // stop test execution after 5 failed assertions
	context.setOption("order-by", "name");            // sort the test cases by their name

	context.applyCommandLine(argc, argv);

	// overrides
	context.setOption("no-breaks", true);             // don't break in the debugger when assertions fail

	int res = context.run(); // run

	/*if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
		return res;          // propagate the result of the tests*/

	spdlog::set_level(spdlog::level::debug);
	spdlog::set_pattern("[%H:%M:%S] [%n] [%^---%L---%$] %v");

	auto console = spdlog::stdout_color_mt("console");
	auto err_logger = spdlog::stderr_color_mt("stderr");
	spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");

	std::cout << "SMETHINGWV" << std::endl;
	spdlog::info("Application Starting");

	auto logger2 = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
	logger2->info("SOMETHING FOR DA FILES");


	//Server server((RequestHandler()));
	//server.startServer("http://127.0.0.1:1234/api"s);

	//char x;
	//std::cin >> x;

	//server.stopServer();

	return 0;
}