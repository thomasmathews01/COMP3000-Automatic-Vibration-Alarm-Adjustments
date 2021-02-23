#pragma once
#include <mutex>
#include <map>
#include <utility>
#include <IClock.h>
#include "IDataAccess.h"
#include "Helpers/sqlite3pp.h"
#include "IDatabaseFactory.h"

class DataStorage : public IDataAccess {
public:
    explicit DataStorage(const std::shared_ptr<IDatabaseFactory>& factory, std::shared_ptr<IClock>  clock) : database(factory->get_database()), clock(std::move(clock)) {}

    [[nodiscard]] std::vector<std::pair<time_point_t, float>> get_data(int channel, int type, time_point_t start, time_point_t end) const noexcept final;
    [[nodiscard]] std::pair<time_point_t, float> get_last_data_point_before(int channel, int type, time_point_t time) const noexcept final;
    [[nodiscard]] std::vector<float> get_data_points_only(int channel, int type, time_point_t start, time_point_t end) const noexcept final;
    [[nodiscard]] std::vector<std::pair<int, std::string>> get_data_types_available_for_channel(int channel_id) const noexcept final;
    [[nodiscard]] std::vector<int> get_all_data_types() const noexcept final;
    [[nodiscard]] time_point_t get_earliest_data_point_for_machine(int machine_id) const noexcept final;
    [[nodiscard]] time_point_t get_latest_data_point_for_machine(int machine_id) const noexcept final;

private:
    const std::shared_ptr<sqlite3pp::database> database; // This is a shared pointer for testing purposes, but should never be accessed by anyone other than this class, with the mutex, unless they take ownership whilst they use it, and we can't access it in that period.
    const std::shared_ptr<IClock> clock;
};