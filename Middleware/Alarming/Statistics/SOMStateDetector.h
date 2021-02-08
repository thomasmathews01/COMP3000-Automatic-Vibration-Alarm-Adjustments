#pragma once
#include <IDataAccess.h>
#include <IClock.h>
#include <utility>
#include <map>
#include <IStateStorage.h>
#include "SOM.h"
#include <IConfigurationAccess.h>

using namespace std::chrono_literals;
using map_type_t = SOM<100, 100, 1000>;

class SOMStateDetector {
public:
	explicit SOMStateDetector(int machine_number, std::shared_ptr<IDataAccess> data_storage, std::shared_ptr<IClock> clock, std::shared_ptr<IStateStorage> state_storage);

	void update() noexcept;
private:
	const int machine_id;
	static constexpr auto minimum_model_representation = 0.95; // If less than 95% of points are accounted for by the model, we need to retrain the model.
	static constexpr auto minimum_time_before_waiting_to_retrain = 1h;
	const std::shared_ptr<IDataAccess> data_storage;
	const std::shared_ptr<IConfigurationAccess> configuration_storage;
	const std::shared_ptr<IStateStorage> state_storage;
	const std::shared_ptr<IClock> clock;

	map_type_t underlying_map;
	std::array<std::map<int , int>, map_type_t::map_size * map_type_t::map_size> state_interpretation;
	std::vector<std::pair<time_point_t, int>> time_mapping; // This gives us the individual node linked to a specific time, which then enables remapping of states without actually looking at the raw data
	int points_contained_within_model = 0;
	time_point_t time_of_last_point_within_model = time_point_t(0s);

	[[nodiscard]] bool requires_update() const noexcept;
	void retrain() noexcept;
	[[nodiscard]] int get_current_state() const noexcept;
	void publish(int state, time_point_t time) const noexcept;
	void remap_states() noexcept;
	void cache_the_things(const std::vector<std::pair<time_point_t, map_type_t::som_point_t>>& points) noexcept;
	void generate_new_model_and_update_values(const std::vector<std::pair<time_point_t, map_type_t::som_point_t>>& data_points);
};
