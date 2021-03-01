#include "Server.h"
#include "SerialisableTypes/Generators.h"
#include "CudaAdder.h"

void Server::start() {
	CROW_ROUTE(server_app, "/")([](){ return "Hello world"; });
	CROW_ROUTE(server_app, "/CPUMatrixAddition").methods("POST"_method)([](auto req){ return Server::matrix_addition_cpu(req); });
	CROW_ROUTE(server_app, "/GPUMatrixAddition").methods("POST"_method)([](auto req){ return Server::matrix_addition_gpu(req); });
	server_app.port(8765).multithreaded().run();
}

void Server::stop() {
	server_app.stop();
}

/// Basic CPU based 1D matrix addition  
/// \param req crow request that contains a json object with 2 matrices.
/// \return json string with a single json matrix  response
std::string Server::matrix_addition_cpu(const crow::request& req) {
	const quicktype::MatrixInput data = nlohmann::json::parse(req.body);
	std::vector<float> output;

	std::transform(data.get_first().cbegin(), data.get_first().cend(), data.get_second().cbegin(), std::back_inserter(output), std::plus<>());

	quicktype::MatrixResponse response;
	response.set_answer(output);

	return nlohmann::to_json(response).dump(4);
}

/// Basic GPU based 1D matrix addition
/// \param req crow request that contains a json object with 2 matrices.
/// \return json string with a single json matrix  response
std::string Server::matrix_addition_gpu(const crow::request& req) {
	const quicktype::MatrixInput data = nlohmann::json::parse(req.body);
	const auto output = CudaAdder::add_matrices(data.get_first(), data.get_second());

	quicktype::MatrixResponse response;
	response.set_answer(output);

	return nlohmann::to_json(response).dump(4);
}
