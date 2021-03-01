#pragma once
#include <memory>
#include "Crow/crow.h"

class Server {
public:
	void start();
	void stop();
private:
	crow::SimpleApp server_app;
	std::vector<std::vector<float>> data_points;

	static std::string matrix_addition_cpu(const crow::request& req);
	static std::string matrix_addition_gpu(const crow::request& req);
};
