#pragma once

#include <memory>
#include <thread>
#include <IDatabase.h>
#include <range/v3/all.hpp>

class BasicStatisticsGenerator {
	explicit BasicStatisticsGenerator(std::shared_ptr<IDatabase>) : database(std::move(database)) {}

	void start();

	double get_limits_for_endpoint(const double standard_deviations) { return 0.0; }

private:
	struct unique_endpoint {
		int channel;
		int type;

		double mean = 0.0;
		double standard_devation = 0.0;
		time_point_t last_update_time = time_point_t(0s);

		unique_endpoint(int channel, int type) : channel(channel), type(type) {};
	};


	std::shared_ptr<IDatabase> database;
	std::thread thread;

	std::vector<unique_endpoint> endpoints;

	void update();
	void update_endpoint(unique_endpoint& endpoint);
};


