#pragma once
#include <mutex>
#include <map>
#include "IDatabase.h"
#include "Helpers/sqlite3pp.h"
#include "IDatabaseFactory.h"

class DataStorage : public IDataAccess {
public:
    explicit DataStorage(const std::shared_ptr<IDatabaseFactory>& factory) : database(factory->get_database()) {}

    std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) final;
    std::pair<time_point_t, float> get_last_data_point_before(int channel, int type, time_point_t time) final;
    std::vector<float> get_data_points_only(int channel, int type, time_point_t start, time_point_t end) final;
    std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) final;
    std::vector<int> get_all_data_types() final;
    time_point_t get_earliest_data_point_for_machine(int machine_id) final;

private:
    std::shared_ptr<sqlite3pp::database> database; // This is a shared pointer for testing purposes, but should never be accessed by anyone other than this class, with the mutex, unless they take ownership whilst they use it, and we can't access it in that period.
};