#pragma once

#include <mutex>
#include "IDatabase.h"
#include "Helpers/sqlite3pp.h"

constexpr auto default_database_location = "/Users/thomasmathews/Documents/VibrationData.db";

class Database : public IDatabase {
public:
	std::vector<site> get_site_data() override;
	std::mutex database_access_mutex;
	void set_up_database_connection();

	sqlite3pp::database database;
	void populate_sites_machine_information(site& site) final;
	void populate_channel_information_for_a_machine(machine& machine) final;
	std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) final;
	std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) final;
	virtual int get_machine_id_from_channel_id(int channel_id) override;
};


