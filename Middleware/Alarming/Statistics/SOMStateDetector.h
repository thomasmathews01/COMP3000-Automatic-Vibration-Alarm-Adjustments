#pragma once
#include <IDataAccess.h>
#include <IClock.h>
#include <utility>
#include <map>
#include <IStateStorage.h>
#include "SOM.h"
using namespace std::chrono_literals;

class SOMStateDetector {
public:
	explicit SOMStateDetector(int machine_number, std::shared_ptr<IDataAccess> data_storage, std::shared_ptr<IClock> clock, std::shared_ptr<IStateStorage> state_storage);

	void update() noexcept;
private:
	const int machine;
	static constexpr auto minimum_model_representation = 0.95; // If less than 95% of points are accounted for by the model, we need to retrain the model.
	static constexpr auto minimum_time_before_waiting_to_retrain = 1h;
	const std::shared_ptr<IDataAccess> data_storage;
	const std::shared_ptr<IStateStorage> state_storage;
	const std::shared_ptr<IClock> clock;

	SOM<100, 100, 1000> underlying_map;
	std::array<std::map<int , int>, 100 * 100> state_interpretation;
	int points_contained_within_model = 0;
	time_point_t time_of_last_point_within_model = time_point_t(0s);

	[[nodiscard]] bool requires_update() const noexcept;
	void retrain() noexcept;
	[[nodiscard]] int get_current_state() const noexcept;
	void publish(int state) const noexcept;
	void remap_states() noexcept;
};
