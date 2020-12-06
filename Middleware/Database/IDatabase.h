#pragma once

#include <string>
#include <vector>
#include <chrono>
#include "../REST Interface/Model/types.h"

using namespace std::chrono;
using time_point_t = system_clock::time_point;

class IDatabase {
public:
	virtual std::vector<site> get_site_data() = 0;
	virtual void populate_sites_machine_information(site& site) = 0;
	virtual void populate_channel_information_for_a_machine(machine& machine) = 0;
	virtual std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) = 0;
	virtual std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) = 0;
	virtual int get_machine_id_from_channel_id(int channel_id) = 0;
};


